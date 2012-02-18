#include <sge/opencl/program/object.hpp>
#include <sge/opencl/program/file_to_source_string_sequence.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <flakelib/program/create_from_context.hpp>
#include <flakelib/program/context.hpp>
#include <fcppt/optional_impl.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/cref.hpp>
#include <fcppt/text.hpp>

flakelib::program::unique_ptr
flakelib::program::create_from_context(
	program::context const &_context,
	fcppt::filesystem::path const &_file_path)
{
	return
		fcppt::make_unique_ptr<sge::opencl::program::object>(
			fcppt::ref(
				_context.command_queue().context()),
			fcppt::cref(
				sge::opencl::program::file_to_source_string_sequence(
					_file_path)),
			fcppt::cref(
				sge::opencl::program::optional_build_parameters(
					sge::opencl::program::build_parameters()
						.options(
							_context.build_options().get()))));
}
