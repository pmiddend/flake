#include <flakelib/media_path_from_string.hpp>
#include <flakelib/cl/apply_kernel_to_planar_image.hpp>
#include <flakelib/laplace_solver/multigrid.hpp>
#include <flakelib/planar_pool/object.hpp>
#include <flakelib/planar_pool/scoped_lock.hpp>
#include <flakelib/utility/object.hpp>
#include <sge/opencl/command_queue/enqueue_kernel.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/memory_object/image/planar.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <sge/opencl/program/file_to_source_string_sequence.hpp>
#include <fcppt/format.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/assign/make_array.hpp>
#include <fcppt/math/is_power_of_2.hpp>
#include <fcppt/math/dim/arithmetic.hpp>
#include <fcppt/math/dim/comparison.hpp>
#include <fcppt/math/dim/output.hpp>
#include <fcppt/config/external_begin.hpp>
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <fcppt/config/external_end.hpp>


flakelib::laplace_solver::multigrid::multigrid(
	flakelib::planar_pool::object &_planar_cache,
	flakelib::utility::object &_utility,
	sge::opencl::command_queue::object &_command_queue,
	flakelib::build_options const &_build_options,
	laplace_solver::base &_inner_solver,
	laplace_solver::grid_scale const &_grid_scale,
	laplace_solver::termination_size const &_termination_size,
	laplace_solver::debug_output const &_debug_output)
:
	planar_cache_(
		_planar_cache),
	utility_(
		_utility),
	command_queue_(
		_command_queue),
	inner_solver_(
		_inner_solver),
	grid_scale_(
		_grid_scale.get()),
	termination_size_(
		_termination_size.get()),
	debug_output_(
		_debug_output.get()),
	main_program_(
		command_queue_.context(),
		sge::opencl::program::file_to_source_string_sequence(
			flakelib::media_path_from_string(
				FCPPT_TEXT("kernels/multigrid.cl"))),
		sge::opencl::program::optional_build_parameters(
			sge::opencl::program::build_parameters()
				.options(
					_build_options.get()))),
	laplacian_residual_kernel_(
		main_program_,
		sge::opencl::kernel::name(
			"laplacian_residual")),
	downsample_kernel_(
		main_program_,
		sge::opencl::kernel::name(
			"downsample")),
	upsample_kernel_(
		main_program_,
		sge::opencl::kernel::name(
			"upsample_")),
	add_kernel_(
		main_program_,
		sge::opencl::kernel::name(
			"add")),
	additional_planar_data_()
{
}

void
flakelib::laplace_solver::multigrid::solve(
	laplace_solver::rhs const &_rhs,
	laplace_solver::destination const &_destination,
	laplace_solver::initial_guess const &_initial_guess,
	laplace_solver::boundary const &_boundary)
{
	FCPPT_ASSERT_PRE(
		_destination.get().size()[0] == _destination.get().size()[1]);

	FCPPT_ASSERT_PRE(
		_destination.get().size() == _initial_guess.get().size());

	FCPPT_ASSERT_PRE(
		_destination.get().size() == _boundary.get().size());

	FCPPT_ASSERT_PRE(
		fcppt::math::is_power_of_2(
			_destination.get().size()[0]));

	sge::opencl::memory_object::size_type const size =
		_destination.get().size()[0];

	flakelib::planar_pool::scoped_lock
		p0(
			planar_cache_,
			size),
		first_smoothing(
			planar_cache_,
			size);

	// Initial guess
	utility_.copy_image(
		utility::from(
			_initial_guess.get()),
		utility::to(
			p0.value()),
		utility::multiplier(
			1.0f));

	// Solve the original problem: Ax = f, retrieve approximate solution:
	// x^(n)
	inner_solver_.solve(
		_rhs,
		laplace_solver::destination(
			first_smoothing.value()),
		laplace_solver::initial_guess(
			p0.value()),
		_boundary);

	this->copy_to_planar_data(
		first_smoothing.value(),
		FCPPT_TEXT("first smoothing"));

	// p1 now contains our first approximation to the solution x^(n)

	if(size == termination_size_)
	{
		// If we're done, solve once more, using our approximate
		// solution as the initial guess, store result directly in
		// _destination
		inner_solver_.solve(
			_rhs,
			_destination,
			laplace_solver::initial_guess(
				first_smoothing.value()),
			_boundary);

		this->copy_to_planar_data(
			_destination.get(),
			FCPPT_TEXT("termination"));

		// We're done!
		return;
	}

	// Calculate residual: r = f - Ax^(n) as a measure of how far away we
	// are from the real solution.
	//
	// The residual is stored in p0
	this->laplacian_residual(
		_rhs,
		laplace_solver::from(
			first_smoothing.value()),
		laplace_solver::to(
			p0.value()),
		_boundary);

	this->copy_to_planar_data(
		p0.value(),
		FCPPT_TEXT("residual"));

	flakelib::planar_pool::scoped_lock
		rd(
			planar_cache_,
			size/2),
		coarse_error(
			planar_cache_,
			size/2),
		coarse_boundary(
			planar_cache_,
			size/2);

	// Downsample the residual, store in rd
	this->downsample(
		laplace_solver::from(
			p0.value()),
		laplace_solver::to(
			rd.value()));

	// Downsample the boundary, store in coarse_boundary
	this->downsample(
		laplace_solver::from(
			_boundary.get()),
		laplace_solver::to(
			coarse_boundary.value()));

	this->copy_to_planar_data(
		rd.value(),
		FCPPT_TEXT("downsampled residual"));

	// To recursively call the solve function again, we need to provide an
	// initial guess. For the recursion steps, we use 0 as the initial
	// guess (or rather, the matrix containing all zeroes)
	flakelib::planar_pool::scoped_lock temporary_initial_guess(
		planar_cache_,
		size/2);

	utility_.null_image(
		temporary_initial_guess.value());

	this->solve(
		laplace_solver::rhs(
			rd.value()),
		laplace_solver::destination(
			coarse_error.value()),
		laplace_solver::initial_guess(
			temporary_initial_guess.value()),
		laplace_solver::boundary(
			coarse_boundary.value()));

	this->copy_to_planar_data(
		coarse_error.value(),
		FCPPT_TEXT("error estimate"));

	if(debug_output_)
	{
		this->laplacian_residual(
			laplace_solver::rhs(
				rd.value()),
			laplace_solver::from(
				coarse_error.value()),
			laplace_solver::to(
				temporary_initial_guess.value()),
			laplace_solver::boundary(
				coarse_boundary.value()));

		this->copy_to_planar_data(
			temporary_initial_guess.value(),
			FCPPT_TEXT("error estimate residual"));
	}

	this->upsample(
		laplace_solver::from(
			coarse_error.value()),
		laplace_solver::to(
			p0.value()));

	this->copy_to_planar_data(
		p0.value(),
		FCPPT_TEXT("upsampled error"));

	flakelib::planar_pool::scoped_lock h(
		planar_cache_,
		size);

	this->add(
		laplace_solver::from(
			first_smoothing.value()),
		laplace_solver::from(
			p0.value()),
		laplace_solver::to(
			h.value()));
//		laplace_solver::to(
//			_destination.get()));

	this->copy_to_planar_data(
		h.value(),
//		_destination.get(),
		FCPPT_TEXT("error corrected"));

	inner_solver_.solve(
		_rhs,
		_destination,
		laplace_solver::initial_guess(
			h.value()),
		_boundary);

	this->copy_to_planar_data(
		_destination.get(),
		FCPPT_TEXT("final smoothing"));

	if(debug_output_)
	{
		this->laplacian_residual(
			_rhs,
			laplace_solver::from(
				_destination.get()),
			laplace_solver::to(
				p0.value()),
			_boundary);

		this->copy_to_planar_data(
			p0.value(),
			FCPPT_TEXT("final residual"));
	}
}

flakelib::additional_planar_data const &
flakelib::laplace_solver::multigrid::additional_planar_data() const
{
	return additional_planar_data_;
}

flakelib::laplace_solver::multigrid::~multigrid()
{
}

void
flakelib::laplace_solver::multigrid::laplacian_residual(
	laplace_solver::rhs const &_rhs,
	laplace_solver::from const &_from,
	laplace_solver::to const &_to,
	laplace_solver::boundary const &_boundary)
{
	FCPPT_ASSERT_PRE(
		_from.get().size() == _to.get().size());

	FCPPT_ASSERT_PRE(
		_rhs.get().size() == _to.get().size());

	laplacian_residual_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		_rhs.get());

	laplacian_residual_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		_boundary.get());

	laplacian_residual_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		_from.get());

	laplacian_residual_kernel_.argument(
		sge::opencl::kernel::argument_index(
			3),
		_to.get());

	laplacian_residual_kernel_.argument(
		sge::opencl::kernel::argument_index(
			4),
		grid_scale_);

	flakelib::cl::apply_kernel_to_planar_image(
		laplacian_residual_kernel_,
		command_queue_,
		_to.get());
}

// from = big
// to = small
void
flakelib::laplace_solver::multigrid::downsample(
	laplace_solver::from const &_from,
	laplace_solver::to const &_to)
{
	FCPPT_ASSERT_PRE(
		_from.get().size() > _to.get().size());

	downsample_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		_from.get());

	downsample_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		_to.get());

	flakelib::cl::apply_kernel_to_planar_image(
		downsample_kernel_,
		command_queue_,
		_to.get());
}

// from = small
// to = big
void
flakelib::laplace_solver::multigrid::upsample(
	laplace_solver::from const &_from,
	laplace_solver::to const &_to)
{
	FCPPT_ASSERT_PRE(
		_from.get().size() < _to.get().size());

	upsample_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		_from.get());

	upsample_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		_to.get());

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		upsample_kernel_,
		fcppt::assign::make_array<std::size_t>
			(_to.get().size()[0])
			(_to.get().size()[1]).container(),
		fcppt::assign::make_array<std::size_t>
			(8)
			(8).container());
}

void
flakelib::laplace_solver::multigrid::add(
	laplace_solver::from const &_from0,
	laplace_solver::from const &_from1,
	laplace_solver::to const &_to)
{
	FCPPT_ASSERT_PRE(
		_from0.get().size() == _from1.get().size());

	FCPPT_ASSERT_PRE(
		_from0.get().size() == _to.get().size());

	add_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		_from0.get());

	add_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		_from1.get());

	add_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		_to.get());

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		add_kernel_,
		fcppt::assign::make_array<std::size_t>
			(_to.get().size()[0])
			(_to.get().size()[1]).container(),
		fcppt::assign::make_array<std::size_t>
			(8)
			(8).container());
}

void
flakelib::laplace_solver::multigrid::copy_to_planar_data(
	sge::opencl::memory_object::image::planar &_image,
	fcppt::string const &_description)
{
	if(!debug_output_)
		return;

	sge::opencl::memory_object::image::planar &temp =
		planar_cache_.get(
			_image.size());

	planar_cache_.lock(
		temp);

	utility_.copy_image(
		utility::from(
			_image),
		utility::to(
			temp),
		utility::multiplier(
			10.0f));

	additional_planar_data_.push_back(
		flakelib::additional_planar_data::value_type(
			(fcppt::format(FCPPT_TEXT("%1%: %2%\nN: %3%"))
				% _image.size()[0]
				% _description
				% utility_.frobenius_norm(_image)).str(),
			flakelib::planar_object(
				&temp)));
}
