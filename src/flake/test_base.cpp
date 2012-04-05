#include <flake/media_path_from_string.hpp>
#include <flake/test_base.hpp>
#include <flake/notifications/object.hpp>
#include <flake/time_modifier/object.hpp>
#include <flakelib/log.hpp>
#include <flakelib/scoped_frame_limiter.hpp>
#include <flakelib/utf8_file_to_fcppt_string.hpp>
#include <flakelib/buffer_pool/object.hpp>
#include <flakelib/cl/cflags.hpp>
#include <flakelib/cl/compiler_flags.hpp>
#include <flakelib/cl/program_context.hpp>
#include <sge/charconv/create_system.hpp>
#include <sge/charconv/system.hpp>
#include <sge/font/metrics.hpp>
#include <sge/font/system.hpp>
#include <sge/log/global_context.hpp>
#include <sge/media/all_extensions.hpp>
#include <sge/opencl/single_device_system/object.hpp>
#include <sge/opencl/single_device_system/parameters.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <sge/parse/json/parse_string_exn.hpp>
#include <sge/parse/json/string_to_path.hpp>
#include <sge/renderer/active_target.hpp>
#include <sge/renderer/no_multi_sampling.hpp>
#include <sge/renderer/parameters.hpp>
#include <sge/renderer/scoped_block.hpp>
#include <sge/renderer/target_base.hpp>
#include <sge/systems/font.hpp>
#include <sge/systems/image2d.hpp>
#include <sge/systems/input.hpp>
#include <sge/systems/instance.hpp>
#include <sge/systems/list.hpp>
#include <sge/systems/renderer.hpp>
#include <sge/systems/window.hpp>
#include <sge/viewport/fill_on_resize.hpp>
#include <sge/viewport/manager.hpp>
#include <sge/window/object.hpp>
#include <sge/window/parameters.hpp>
#include <sge/window/system.hpp>
#include <sge/window/title.hpp>
#include <awl/main/exit_code.hpp>
#include <awl/main/exit_failure.hpp>
#include <awl/main/exit_success.hpp>
#include <awl/main/function_context.hpp>
#include <fcppt/cref.hpp>
#include <fcppt/exception.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/text.hpp>
#include <fcppt/to_std_string.hpp>
#include <fcppt/container/bitfield/object_impl.hpp>
#include <fcppt/filesystem/path_to_string.hpp>
#include <fcppt/io/cerr.hpp>
#include <fcppt/log/headers.hpp>
#include <fcppt/log/location.hpp>
#include <fcppt/math/box/object_impl.hpp>
#include <fcppt/tr1/functional.hpp>
#include <fcppt/config/external_begin.hpp>
#include <exception>
#include <iostream>
#include <ostream>
#include <fcppt/config/external_end.hpp>


awl::main::exit_code const
flake::test_base::run()
{
	systems_->window().show();

	while(systems_->window_system().poll())
	{
		flakelib::scoped_frame_limiter frame_limiter(
			desired_fps_);

		this->update();

		if(sge::renderer::active_target(this->renderer()).viewport().get().content())
		{
			sge::renderer::scoped_block scoped_render_block(
				systems_->renderer());

			this->render();
		}
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
	charconv_system_(
		sge::charconv::create_system()),
	configuration_(
		fcppt::make_unique_ptr<sge::parse::json::object>(
			fcppt::cref(
				sge::parse::json::parse_string_exn(
					flakelib::utf8_file_to_fcppt_string(
						*charconv_system_,
						flake::media_path_from_string(
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
										FCPPT_TEXT("tests/window-size"))))).dont_show())
						(sge::systems::renderer(
							sge::renderer::parameters(
								sge::renderer::visual_depth::depth32,
								sge::renderer::depth_stencil_buffer::off,
								sge::renderer::vsync::on,
								sge::renderer::no_multi_sampling),
							sge::viewport::fill_on_resize()))
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
					flakelib::cl::cflags()+
					" "+
					fcppt::to_std_string(
						sge::parse::json::find_and_convert_member<fcppt::string>(
							this->configuration(),
							sge::parse::json::string_to_path(
								FCPPT_TEXT("tests/opencl-compiler-flags")))+
						FCPPT_TEXT(" -I")+
						fcppt::filesystem::path_to_string(
							flake::media_path_from_string(
								FCPPT_TEXT("kernels")))))))),
	buffer_pool_(
		fcppt::make_unique_ptr<flakelib::buffer_pool::object>(
			fcppt::ref(
				this->opencl_system().context()))),
	desired_fps_(
		sge::parse::json::find_and_convert_member<flakelib::scoped_frame_limiter::fps_type>(
			this->configuration(),
			sge::parse::json::string_to_path(
				FCPPT_TEXT("tests/desired-fps")))),
	viewport_connection_(
		systems_->viewport_manager().manage_callback(
			std::tr1::bind(
				&test_base::viewport_callback,
				this))),
	notifications_(
		fcppt::make_unique_ptr<flake::notifications::object>(
			fcppt::ref(
				this->renderer()),
			sge::font::metrics_shared_ptr(
				this->font_system().create_font(
					flake::media_path_from_string(
						FCPPT_TEXT("fonts/notifications.ttf")),
					sge::parse::json::find_and_convert_member<sge::font::size_type>(
						this->configuration(),
						sge::parse::json::string_to_path(
							FCPPT_TEXT("tests/notification-font-size"))))),
			flake::notifications::time_to_live(
				boost::chrono::milliseconds(
					sge::parse::json::find_and_convert_member<boost::chrono::milliseconds::rep>(
						this->configuration(),
						sge::parse::json::string_to_path(
							FCPPT_TEXT("tests/notification-ttl-ms"))))))),
	time_modifier_(
		fcppt::make_unique_ptr<time_modifier::object>(
			fcppt::ref(
				this->keyboard()),
			fcppt::ref(
				*notifications_)))
{
	FCPPT_LOG_DEBUG(
		flakelib::log(),
		fcppt::log::_
			<< FCPPT_TEXT("Program initialized, OpenCL compiler flags are:\n\n")
			<< fcppt::from_std_string(program_context_->compiler_flags().get()));
}

void
flake::test_base::update()
{
	notifications_->update();
}

void
flake::test_base::render()
{
	notifications_->render();
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

sge::input::mouse::device &
flake::test_base::mouse()
{
	return
		systems_->mouse_collector();
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

void
flake::test_base::post_notification(
	notifications::text const &_text)
{
	notifications_->add_message(
		_text);
}

flake::time_modifier::multiplier const
flake::test_base::current_multiplier() const
{
	return
		time_modifier_->current_multiplier();
}

void
flake::test_base::viewport_callback()
{
}
