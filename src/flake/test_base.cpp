#include <fcppt/cref.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/to_std_string.hpp>
#include <fcppt/filesystem/path_to_string.hpp>
#include <awl/main/exit_code.hpp>
#include <awl/main/exit_failure.hpp>
#include <awl/main/exit_success.hpp>
#include <awl/main/function_context.hpp>
#include <exception>
#include <exception>
#include <fcppt/container/bitfield/basic_impl.hpp>
#include <fcppt/exception.hpp>
#include <fcppt/io/cerr.hpp>
#include <fcppt/log/location.hpp>
#include <fcppt/text.hpp>
#include <fcppt/tr1/functional.hpp>
#include <flakelib/media_path_from_string.hpp>
#include <flakelib/utf8_file_to_fcppt_string.hpp>
#include <flakelib/scoped_frame_limiter.hpp>
#include <flake/planar/tests/simple.hpp>
#include <flake/test_base.hpp>
#include <iostream>
#include <ostream>
#include <sge/log/global_context.hpp>
#include <sge/media/all_extensions.hpp>
#include <sge/opencl/single_device_system/parameters.hpp>
#include <sge/opencl/single_device_system/object.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <sge/parse/json/parse_string_exn.hpp>
#include <sge/parse/json/string_to_path.hpp>
#include <sge/renderer/no_multi_sampling.hpp>
#include <sge/renderer/parameters.hpp>
#include <sge/renderer/scoped_block.hpp>
#include <sge/systems/font.hpp>
#include <sge/systems/image2d.hpp>
#include <sge/systems/instance.hpp>
#include <sge/systems/input.hpp>
#include <sge/systems/list.hpp>
#include <sge/systems/renderer.hpp>
#include <sge/systems/window.hpp>
#include <sge/viewport/center_on_resize.hpp>
#include <sge/window/parameters.hpp>
#include <sge/window/system.hpp>
#include <sge/window/title.hpp>

awl::main::exit_code const
flake::test_base::run()
{
	while(systems_->window_system().poll())
	{
		flakelib::scoped_frame_limiter frame_limiter(
			);

		this->update();

		sge::renderer::scoped_block scoped_render_block(
			systems_->renderer());

		this->render();
	}

	return
		systems_->window_system().exit_code();
}

flake::test_base::~test_base()
{
}

flake::test_base::test_base(
	awl::main::function_context const &,
	sge::window::title const &_window_title,
	sge::systems::cursor_option_field const &_cursor_options)
:
	configuration_(
		fcppt::make_unique_ptr<sge::parse::json::object>(
			fcppt::cref(
				sge::parse::json::parse_string_exn(
					flakelib::utf8_file_to_fcppt_string(
						flakelib::media_path_from_string(
							FCPPT_TEXT("config.json"))))))),
	systems_(
		fcppt::make_unique_ptr<sge::systems::instance>(
			fcppt::cref(
				sge::systems::list()
					(sge::systems::image2d(
						sge::image::capabilities_field::null(),
						sge::media::all_extensions))
						(sge::systems::window(
							sge::window::parameters(
								_window_title,
								sge::parse::json::find_and_convert_member<sge::window::dim>(
									*configuration_,
									sge::parse::json::string_to_path(
										FCPPT_TEXT("tests/window-size"))))))
						(sge::systems::renderer(
							sge::renderer::parameters(
								sge::renderer::visual_depth::depth32,
								sge::renderer::depth_stencil_buffer::off,
								sge::renderer::vsync::on,
								sge::renderer::no_multi_sampling),
							sge::viewport::center_on_resize(
								sge::parse::json::find_and_convert_member<sge::window::dim>(
									*configuration_,
									sge::parse::json::string_to_path(
										FCPPT_TEXT("tests/window-size"))))))
						(sge::systems::font())
						(sge::systems::input(
							sge::systems::input_helper_field(
								sge::systems::input_helper::keyboard_collector)
								| sge::systems::input_helper::cursor_demuxer
								| sge::systems::input_helper::mouse_collector,
							_cursor_options))))),
	opencl_system_(
		fcppt::make_unique_ptr<sge::opencl::single_device_system::object>(
			fcppt::cref(
				sge::opencl::single_device_system::parameters()
					.renderer(
						this->renderer())
					.prefer_gpu(
						true)))),
	program_context_(
		fcppt::make_unique_ptr<flakelib::cl::program_context>(
			fcppt::ref(
				this->opencl_system().command_queue()),
			fcppt::cref(
				flakelib::cl::compiler_flags(
					fcppt::to_std_string(
						sge::parse::json::find_and_convert_member<fcppt::string>(
							this->configuration(),
							sge::parse::json::string_to_path(
								FCPPT_TEXT("tests/opencl-compiler-flags")))+
						FCPPT_TEXT(" -I")+
						fcppt::filesystem::path_to_string(
							flakelib::media_path_from_string(
								FCPPT_TEXT("kernels")))))))),
	buffer_pool_(
		fcppt::make_unique_ptr<flakelib::buffer_pool::object>(
			fcppt::ref(
				this->opencl_system().context()))),
	desired_fps_(
		sge::parse::json::find_and_convert_member<flakelib::scoped_frame_limiter::fps_type>(
			this->configuration(),
			sge::parse::json::string_to_path(
				FCPPT_TEXT("tests/desired-fps"))))
{
}

sge::parse::json::object const &
flake::test_base::configuration() const
{
	return
		*configuration_;
}

sge::renderer::device &
flake::test_base::renderer()
{
	return
		systems_->renderer();
}

sge::opencl::single_device_system::object &
flake::test_base::opencl_system()
{
	return
		*opencl_system_;
}

sge::input::cursor::object &
flake::test_base::cursor()
{
	return
		systems_->cursor_demuxer();
}

sge::input::keyboard::device &
flake::test_base::keyboard()
{
	return
		systems_->keyboard_collector();
}

sge::font::system &
flake::test_base::font_system()
{
	return
		systems_->font_system();
}

flakelib::cl::program_context const &
flake::test_base::program_context() const
{
	return
		*program_context_;
}

sge::image2d::system &
flake::test_base::image_system()
{
	return
		systems_->image_system();
}

flakelib::buffer_pool::object &
flake::test_base::buffer_pool()
{
	return
		*buffer_pool_;
}

sge::viewport::manager &
flake::test_base::viewport_manager()
{
	return
		systems_->viewport_manager();
}
