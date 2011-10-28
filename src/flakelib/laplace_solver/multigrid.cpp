#include <flakelib/media_path_from_string.hpp>
#include <flakelib/planar_cache.hpp>
#include <flakelib/planar_lock.hpp>
#include <flakelib/cl/apply_kernel_to_planar_image.hpp>
#include <flakelib/laplace_solver/multigrid.hpp>
#include <sge/opencl/command_queue/enqueue_kernel.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/memory_object/image/planar.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <sge/opencl/program/file_to_source_string_sequence.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/assign/make_array.hpp>
#include <fcppt/insert_to_fcppt_string.hpp>
#include <fcppt/math/is_power_of_2.hpp>
#include <fcppt/math/dim/arithmetic.hpp>
#include <fcppt/math/dim/output.hpp>
#include <fcppt/math/dim/comparison.hpp>
#include <fcppt/config/external_begin.hpp>
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <fcppt/config/external_end.hpp>


flakelib::laplace_solver::multigrid::multigrid(
	flakelib::planar_cache &_planar_cache,
	sge::opencl::command_queue::object &_command_queue,
	laplace_solver::base &_inner_solver,
	laplace_solver::grid_scale const &_grid_scale)
:
	planar_cache_(
		_planar_cache),
	command_queue_(
		_command_queue),
	inner_solver_(
		_inner_solver),
	grid_scale_(
		_grid_scale.get()),
	utility_program_(
		command_queue_.context(),
		sge::opencl::program::file_to_source_string_sequence(
			flakelib::media_path_from_string(
				FCPPT_TEXT("kernels/utility.cl"))),
		sge::opencl::program::build_parameters()),
	copy_image_kernel_(
		utility_program_,
		sge::opencl::kernel::name(
			"copy_image")),
	null_image_kernel_(
		utility_program_,
		sge::opencl::kernel::name(
			"null_image")),
	main_program_(
		command_queue_.context(),
		sge::opencl::program::file_to_source_string_sequence(
			flakelib::media_path_from_string(
				FCPPT_TEXT("kernels/multigrid.cl"))),
		sge::opencl::program::build_parameters()),
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
	laplace_solver::initial_guess const &_initial_guess)
{
	FCPPT_ASSERT_PRE(
		_destination.get().size()[0] == _destination.get().size()[1]);

	FCPPT_ASSERT_PRE(
		fcppt::math::is_power_of_2(
			_destination.get().size()[0]));

	std::cout << "Solve iteration with size: " << _rhs.get().size() << "\n";

	sge::opencl::memory_object::size_type const size =
		_destination.get().size()[0];

	flakelib::planar_lock
		p0(
			planar_cache_,
			size),
		p1(
			planar_cache_,
			size);

	// Initial guess
	this->copy_image(
		laplace_solver::from(
			_initial_guess.get()),
		laplace_solver::to(
			p0.value()));

	inner_solver_.solve(
		_rhs,
		laplace_solver::destination(
			p1.value()),
		laplace_solver::initial_guess(
			p0.value()));

	this->copy_to_planar_data(
		p1.value(),
		FCPPT_TEXT("first smoothing"));

	if(size == 32)
	{
		std::cout << "Small enough, bailing out\n";

		inner_solver_.solve(
			_rhs,
			laplace_solver::destination(
				p0.value()),
			laplace_solver::initial_guess(
				_destination.get()));

		this->copy_to_planar_data(
			p0.value(),
			FCPPT_TEXT("termination"));
		return;
	}

	this->laplacian_residual(
		_rhs,
		laplace_solver::from(
			p1.value()),
		laplace_solver::to(
			p0.value()));

	this->copy_to_planar_data(
		p0.value(),
		FCPPT_TEXT("residual"));

	flakelib::planar_lock
		rd(
			planar_cache_,
			size/2),
		coarse_rhs(
			planar_cache_,
			size/2);

	this->downsample(
		laplace_solver::from(
			p0.value()),
		laplace_solver::to(
			rd.value()));

	this->copy_to_planar_data(
		rd.value(),
		FCPPT_TEXT("downsampled residual"));

	flakelib::planar_lock temporary_initial_guess(
		planar_cache_,
		size/2);

	this->null_image(
		temporary_initial_guess.value());

	this->solve(
		laplace_solver::rhs(
			rd.value()),
		laplace_solver::destination(
			coarse_rhs.value()),
		laplace_solver::initial_guess(
			temporary_initial_guess.value()));

	this->copy_to_planar_data(
		coarse_rhs.value(),
		FCPPT_TEXT("error estimate"));

	this->upsample(
		laplace_solver::from(
			coarse_rhs.value()),
		laplace_solver::to(
			p0.value()));

	this->copy_to_planar_data(
		p0.value(),
		FCPPT_TEXT("upsampled error"));

	flakelib::planar_lock h(
		planar_cache_,
		size);

	this->add(
		laplace_solver::from(
			p0.value()),
		laplace_solver::from(
			p1.value()),
		laplace_solver::to(
			h.value()));

	this->copy_to_planar_data(
		h.value(),
		FCPPT_TEXT("error corrected"));

	inner_solver_.solve(
		_rhs,
		_destination,
		laplace_solver::initial_guess(
			h.value()));
	this->copy_to_planar_data(
		_destination.get(),
		FCPPT_TEXT("final smoothing"));
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
flakelib::laplace_solver::multigrid::null_image(
	sge::opencl::memory_object::image::planar &_f)
{
	null_image_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		_f);

	flakelib::cl::apply_kernel_to_planar_image(
		null_image_kernel_,
		command_queue_,
		_f);
}

void
flakelib::laplace_solver::multigrid::copy_image(
	laplace_solver::from const &_from,
	laplace_solver::to const &_to)
{
	copy_image_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		_from.get());

	copy_image_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		_to.get());

	flakelib::cl::apply_kernel_to_planar_image(
		copy_image_kernel_,
		command_queue_,
		_from.get());
}

void
flakelib::laplace_solver::multigrid::laplacian_residual(
	laplace_solver::rhs const &_rhs,
	laplace_solver::from const &_from,
	laplace_solver::to const &_to)
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
		_from.get());

	laplacian_residual_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		_to.get());

	laplacian_residual_kernel_.argument(
		sge::opencl::kernel::argument_index(
			3),
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
			(1)
			(1).container());
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
			(1)
			(1).container());
}

void
flakelib::laplace_solver::multigrid::copy_to_planar_data(
	sge::opencl::memory_object::image::planar &_image,
	fcppt::string const &_description)
{
	sge::opencl::memory_object::image::planar &temp =
		planar_cache_.get(
			_image.size());

	planar_cache_.lock(
		temp);

	this->copy_image(
		laplace_solver::from(
			_image),
		laplace_solver::to(
			temp));

	additional_planar_data_.push_back(
		std::make_pair(
			fcppt::insert_to_fcppt_string(_image.size()[0])+FCPPT_TEXT(": ")+_description,
			flakelib::planar_object(
				&temp)));
}
