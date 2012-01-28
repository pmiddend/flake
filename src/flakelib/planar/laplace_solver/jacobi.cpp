#include <flakelib/media_path_from_string.hpp>
#include <flakelib/buffer_pool/planar_lock.hpp>
#include <flakelib/cl/apply_kernel_to_planar_image.hpp>
#include <flakelib/planar/laplace_solver/jacobi.hpp>
#include <sge/opencl/command_queue/enqueue_kernel.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <sge/opencl/program/file_to_source_string_sequence.hpp>
#include <fcppt/text.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/assign/make_array.hpp>
#include <fcppt/math/dim/comparison.hpp>
#include <fcppt/config/external_begin.hpp>
#include <algorithm>
#include <fcppt/config/external_end.hpp>


flakelib::planar::laplace_solver::jacobi::jacobi(
	flakelib::buffer_pool::object &_buffer_cache,
	sge::opencl::command_queue::object &_command_queue,
	flakelib::build_options const &_build_options,
	planar::laplace_solver::iterations const &_iterations)
:
	buffer_cache_(
		_buffer_cache),
	command_queue_(
		_command_queue),
	iterations_(
		_iterations.get()),
	jacobi_program_(
		command_queue_.context(),
		sge::opencl::program::file_to_source_string_sequence(
			flakelib::media_path_from_string(
				FCPPT_TEXT("kernels/planar/jacobi.cl"))),
		sge::opencl::program::optional_build_parameters(
			sge::opencl::program::build_parameters()
				.options(
					_build_options.get()))),
	jacobi_kernel_(
		jacobi_program_,
		sge::opencl::kernel::name(
			"jacobi"))
{
}

void
flakelib::planar::laplace_solver::jacobi::solve(
	planar::laplace_solver::rhs const &_rhs,
	planar::laplace_solver::destination const &_destination,
	planar::laplace_solver::initial_guess const &_initial_guess,
	planar::laplace_solver::boundary const &_boundary)
{
	FCPPT_ASSERT_PRE(
		iterations_ % 2 != 0);

	FCPPT_ASSERT_PRE(
		_rhs.get().size()[0] == _rhs.get().size()[1]);

	FCPPT_ASSERT_PRE(
		_rhs.get().size() == _destination.get().size());

	FCPPT_ASSERT_PRE(
		_initial_guess.get().size() == _destination.get().size());

	flakelib::buffer_pool::planar_lock<cl_float> p0(
		buffer_cache_,
		_rhs.get().size());

	// buffer width
	jacobi_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		static_cast<cl_int>(
			_rhs.get().size()[0]));

	// rhs
	jacobi_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		_rhs.get().buffer());

	// boundary
	jacobi_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		_boundary.get().buffer());

	sge::opencl::memory_object::buffer
		*current_source =
			&_initial_guess.get().buffer(),
		*current_dest =
			&_destination.get().buffer();

	for(iterations::value_type i = 0; i < iterations_; ++i)
	{
		// x
		jacobi_kernel_.argument(
			sge::opencl::kernel::argument_index(
				3),
			*current_source);

		// output
		jacobi_kernel_.argument(
			sge::opencl::kernel::argument_index(
				4),
			*current_dest);

		if(i == 0)
		{
			current_source =
				current_dest;
			current_dest =
				&p0.value().buffer();
		}
		else
		{
			std::swap(
				current_source,
				current_dest);
		}

		sge::opencl::command_queue::enqueue_kernel(
			command_queue_,
			jacobi_kernel_,
			fcppt::assign::make_array<sge::opencl::memory_object::size_type>
				(_rhs.get().size()[0])
				(_rhs.get().size()[1]).container());
	}
}

flakelib::planar::additional_scalar_data const &
flakelib::planar::laplace_solver::jacobi::additional_scalar_data() const
{
	return additional_scalar_data_;
}

flakelib::planar::laplace_solver::jacobi::~jacobi()
{
}
