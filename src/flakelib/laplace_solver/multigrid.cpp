#include <flakelib/media_path_from_string.hpp>
#include <flakelib/planar_cache.hpp>
#include <flakelib/laplace_solver/multigrid.hpp>
#include <sge/opencl/command_queue/enqueue_kernel.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/memory_object/image/planar.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/assign/make_array.hpp>
#include <fcppt/assign/make_container.hpp>
#include <fcppt/io/cifstream.hpp>
#include <fcppt/io/stream_to_string.hpp>
#include <fcppt/math/is_power_of_2.hpp>
#include <fcppt/math/dim/arithmetic.hpp>
#include <fcppt/math/dim/comparison.hpp>
#include <fcppt/config/external_begin.hpp>
#include <algorithm>
#include <cstddef>
#include <fcppt/config/external_end.hpp>


flakelib::laplace_solver::multigrid::multigrid(
	flakelib::planar_cache &_planar_cache,
	sge::opencl::command_queue::object &_command_queue,
	laplace_solver::grid_scale const &_grid_scale)
:
	planar_cache_(
		_planar_cache),
	command_queue_(
		_command_queue),
	grid_scale_(
		_grid_scale.get()),
	jacobi_iterations_(
		5u),
	program_(
		command_queue_.context(),
		fcppt::assign::make_container<sge::opencl::program::source_string_sequence>(
			fcppt::io::stream_to_string(
				*fcppt::make_unique_ptr<fcppt::io::cifstream>(
					flakelib::media_path_from_string(
						FCPPT_TEXT("kernels/multigrid.cl"))))),
		sge::opencl::program::build_parameters()),
	null_image_kernel_(
		program_,
		sge::opencl::kernel::name(
			"null_image")),
	jacobi_kernel_(
		program_,
		sge::opencl::kernel::name(
			"jacobi")),
	laplacian_residual_kernel_(
		program_,
		sge::opencl::kernel::name(
			"laplacian_residual")),
	downsample_kernel_(
		program_,
		sge::opencl::kernel::name(
			"downsample")),
	upsample_kernel_(
		program_,
		sge::opencl::kernel::name(
			"upsample_")),
	add_kernel_(
		program_,
		sge::opencl::kernel::name(
			"add"))
{
}

void
flakelib::laplace_solver::multigrid::solve(
	laplace_solver::rhs const &_rhs,
	laplace_solver::destination const &_destination)
{
	FCPPT_ASSERT_PRE(
		_destination.get().size()[0] == _destination.get().size()[1]);

	FCPPT_ASSERT_PRE(
		fcppt::math::is_power_of_2(
			_destination.get().size()[0]));

	sge::opencl::memory_object::size_type const size =
		_destination.get().size()[0];

	sge::opencl::memory_object::image::planar
		&p0 =
			planar_cache_.get(
				size),
		&p1 =
			planar_cache_.get(
				size);

	// Initial guess
	this->null_image(
		p0);

	this->jacobi(
		laplace_solver::initial_guess(
			p0),
		laplace_solver::destination(
			p1),
		_rhs);

	if(size == 8u)
	{
		this->jacobi(
			laplace_solver::initial_guess(
				_destination.get()),
			laplace_solver::destination(
				p0),
			_rhs);
		return;
	}

	this->laplacian_residual(
		_rhs,
		laplace_solver::from(
			p1),
		laplace_solver::to(
			p0));

	sge::opencl::memory_object::image::planar
		&rd =
			planar_cache_.get(
				size),
		&coarse_rhs =
			planar_cache_.get(
				size);

	this->downsample(
		laplace_solver::from(
			p0),
		laplace_solver::to(
			rd));

	this->solve(
		laplace_solver::rhs(
			rd),
		laplace_solver::destination(
			coarse_rhs));

	this->upsample(
		laplace_solver::from(
			coarse_rhs),
		laplace_solver::to(
			p0));

	sge::opencl::memory_object::image::planar &
		h =
			planar_cache_.get(
				size);

	this->add(
		laplace_solver::from(
			p0),
		laplace_solver::from(
			p1),
		laplace_solver::to(
			h));

	this->jacobi(
		laplace_solver::initial_guess(
			h),
		_destination,
		_rhs);
}

flakelib::laplace_solver::multigrid::~multigrid()
{
}

void
flakelib::laplace_solver::multigrid::null_image(
	sge::opencl::memory_object::image::planar &input)
{
	null_image_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		input);

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		null_image_kernel_,
		fcppt::assign::make_array<std::size_t>
			(input.size()[0])
			(input.size()[1]).container(),
		fcppt::assign::make_array<std::size_t>
			(1)
			(1).container());
}

void
flakelib::laplace_solver::multigrid::jacobi(
	laplace_solver::initial_guess const &_initial_guess,
	laplace_solver::destination const &_destination,
	laplace_solver::rhs const &_rhs)
{
	FCPPT_ASSERT_PRE(
		jacobi_iterations_ % 2 != 0);

	// Alpha
	jacobi_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		-(grid_scale_ * grid_scale_));

	// Beta (rbeta)
	jacobi_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		1.0f/4.0f);

	jacobi_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		_rhs.get());

	sge::opencl::memory_object::image::planar
		*current_source = &_initial_guess.get(),
		*current_dest = &_destination.get();

	for(unsigned i = 0; i < jacobi_iterations_; ++i)
	{
		jacobi_kernel_.argument(
			sge::opencl::kernel::argument_index(
				3),
			*current_source);

		jacobi_kernel_.argument(
			sge::opencl::kernel::argument_index(
				4),
			*current_dest);

		std::swap(
			current_source,
			current_dest);

		sge::opencl::command_queue::enqueue_kernel(
			command_queue_,
			jacobi_kernel_,
			fcppt::assign::make_array<std::size_t>
				(current_source->size()[0])
				(current_source->size()[1]).container(),
			fcppt::assign::make_array<std::size_t>
				(1)
				(1).container());
	}
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

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		laplacian_residual_kernel_,
		fcppt::assign::make_array<std::size_t>
			(_to.get().size()[0])
			(_to.get().size()[1]).container(),
		fcppt::assign::make_array<std::size_t>
			(1)
			(1).container());
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

	sge::opencl::command_queue::enqueue_kernel(
		command_queue_,
		downsample_kernel_,
		fcppt::assign::make_array<std::size_t>
			(_to.get().size()[0])
			(_to.get().size()[1]).container(),
		fcppt::assign::make_array<std::size_t>
			(1)
			(1).container());
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
