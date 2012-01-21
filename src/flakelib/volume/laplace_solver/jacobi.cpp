#include <flakelib/media_path_from_string.hpp>
#include <flakelib/buffer/volume_view.hpp>
#include <flakelib/buffer_pool/volume_lock.hpp>
#include <flakelib/volume/laplace_solver/jacobi.hpp>
#include <sge/opencl/command_queue/enqueue_kernel.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/kernel/name.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <sge/opencl/program/file_to_source_string_sequence.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/assign/make_array.hpp>
#include <fcppt/math/dim/comparison.hpp>


flakelib::volume::laplace_solver::jacobi::jacobi(
	flakelib::buffer_pool::object &_buffer_pool,
	sge::opencl::command_queue::object &_command_queue,
	flakelib::build_options const &_build_options,
	volume::laplace_solver::boundary const &_boundary,
	volume::laplace_solver::iterations const &_iterations)
:
	buffer_pool_(
		_buffer_pool),
	command_queue_(
		_command_queue),
	iterations_(
		_iterations.get()),
	jacobi_program_(
		command_queue_.context(),
		sge::opencl::program::file_to_source_string_sequence(
			flakelib::media_path_from_string(
				FCPPT_TEXT("kernels/volume/jacobi.cl"))),
		sge::opencl::program::optional_build_parameters(
			sge::opencl::program::build_parameters()
				.options(
					_build_options.get()))),
	jacobi_kernel_(
		jacobi_program_,
		sge::opencl::kernel::name(
			"jacobi"))
{
	FCPPT_ASSERT_PRE(
		iterations_ % 2 != 0);

	// boundary
	jacobi_kernel_.argument(
		sge::opencl::kernel::argument_index(
			0),
		_boundary.get().buffer());

}

void
flakelib::volume::laplace_solver::jacobi::solve(
	volume::laplace_solver::rhs const &_rhs,
	volume::laplace_solver::destination const &_destination,
	volume::laplace_solver::initial_guess const &_initial_guess)
{
	FCPPT_ASSERT_PRE(
		_rhs.get().size()[0] == _rhs.get().size()[1] &&
		_rhs.get().size()[1] == _rhs.get().size()[2]);

	FCPPT_ASSERT_PRE(
		_rhs.get().size() == _destination.get().size());

	FCPPT_ASSERT_PRE(
		_initial_guess.get().size() == _destination.get().size());

	flakelib::buffer_pool::volume_lock<cl_float> p0(
		buffer_pool_,
		_rhs.get().size());

	// buffer width
	jacobi_kernel_.argument(
		sge::opencl::kernel::argument_index(
			1),
		static_cast<cl_int>(
			_rhs.get().size()[0]));

	// rhs
	jacobi_kernel_.argument(
		sge::opencl::kernel::argument_index(
			2),
		_rhs.get().buffer());

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
				(_rhs.get().size()[1])
				(_rhs.get().size()[2]).container());
	}
}

flakelib::volume::laplace_solver::jacobi::~jacobi()
{
}
