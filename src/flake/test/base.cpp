#include <flake/media_path_from_string.hpp>
#include <flake/notifications/object.hpp>
#include <flake/test/base.hpp>
#include <flake/test/update_features_from_json.hpp>
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
#include <sge/input/keyboard/device.hpp>
#include <sge/input/keyboard/key_event.hpp>
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
#include <fcppt/assert/unreachable_message.hpp>
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
flake::test::base::run()
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

flake::test::base::~base()
{
}

flake::test::base::base(
	awl::main::function_context const &,
	sge::window::title const &_window_title,
	test::json_identifier const &_json_identifier,
	test::feature_sequence const &_features,
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
	local_configuration_(
		sge::parse::json::find_and_convert_member<sge::parse::json::object const>(
			*configuration_,
			sge::parse::json::string_to_path(
				FCPPT_TEXT("tests/")+_json_identifier.get()))),
	features_(
		test::update_features_from_json(
			_features,
			sge::parse::json::find_and_convert_member<sge::parse::json::object const>(
				local_configuration_,
				sge::parse::json::string_to_path(
					FCPPT_TEXT("features"))))),
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
								sge::renderer::depth_stencil_buffer::d24,
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
							*configuration_,
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
			*configuration_,
			sge::parse::json::string_to_path(
				FCPPT_TEXT("tests/desired-fps")))),
	viewport_connection_(
		systems_->viewport_manager().manage_callback(
			std::tr1::bind(
				&test::base::viewport_callback,
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
						*configuration_,
						sge::parse::json::string_to_path(
							FCPPT_TEXT("tests/notification-font-size"))))),
			flake::notifications::time_to_live(
				boost::chrono::milliseconds(
					sge::parse::json::find_and_convert_member<boost::chrono::milliseconds::rep>(
						*configuration_,
						sge::parse::json::string_to_path(
							FCPPT_TEXT("tests/notification-ttl-ms"))))))),
	time_modifier_(
		fcppt::make_unique_ptr<time_modifier::object>(
			fcppt::ref(
				this->keyboard()),
			fcppt::ref(
				*notifications_),
			sge::parse::json::find_and_convert_member<bool>(
				local_configuration_,
				sge::parse::json::string_to_path(
					FCPPT_TEXT("features/paused"))))),
	key_callback_connection_(
		this->keyboard().key_callback(
			std::tr1::bind(
				&base::key_callback,
				this,
				std::tr1::placeholders::_1)))
{
	FCPPT_LOG_DEBUG(
		flakelib::log(),
		fcppt::log::_
			<< FCPPT_TEXT("Program initialized, OpenCL compiler flags are:\n\n")
			<< fcppt::from_std_string(program_context_->compiler_flags().get()));
}

void
flake::test::base::update()
{
	notifications_->update();
}

void
flake::test::base::render()
{
	notifications_->render();
}

sge::parse::json::object const &
flake::test::base::configuration() const
{
	return
		local_configuration_;
}

sge::renderer::device &
flake::test::base::renderer()
{
	return
		systems_->renderer();
}

sge::opencl::single_device_system::object &
flake::test::base::opencl_system()
{
	return
		*opencl_system_;
}

sge::input::cursor::object &
flake::test::base::cursor()
{
	return
		systems_->cursor_demuxer();
}

sge::input::mouse::device &
flake::test::base::mouse()
{
	return
		systems_->mouse_collector();
}

sge::input::keyboard::device &
flake::test::base::keyboard()
{
	return
		systems_->keyboard_collector();
}

sge::font::system &
flake::test::base::font_system()
{
	return
		systems_->font_system();
}

flakelib::cl::program_context const &
flake::test::base::program_context() const
{
	return
		*program_context_;
}

sge::image2d::system &
flake::test::base::image_system()
{
	return
		systems_->image_system();
}

flakelib::buffer_pool::object &
flake::test::base::buffer_pool()
{
	return
		*buffer_pool_;
}

sge::viewport::manager &
flake::test::base::viewport_manager()
{
	return
		systems_->viewport_manager();
}

void
flake::test::base::post_notification(
	notifications::text const &_text)
{
	notifications_->add_message(
		_text);
}

flake::time_modifier::multiplier const
flake::test::base::current_multiplier() const
{
	return
		time_modifier_->current_multiplier();
}

bool
flake::test::base::feature_active(
	test::json_identifier const &_identifier) const
{
	for(
		test::feature_sequence::const_iterator it = features_.begin();
		it != features_.end();
		++it)
		if(it->json_identifier() == _identifier)
			return it->is_active();

	FCPPT_ASSERT_UNREACHABLE_MESSAGE(
		FCPPT_TEXT("Feature with unknown identifier"));
}

void
flake::test::base::viewport_callback()
{
}

namespace
{
fcppt::string const
bool_to_string(
	bool const b)
{
	return
		b
		?
			fcppt::string(FCPPT_TEXT("on"))
		:
			fcppt::string(FCPPT_TEXT("off"));
}
}

void
flake::test::base::key_callback(
	sge::input::keyboard::key_event const &e)
{
	if(!e.pressed())
		return;

	for(
		test::feature_sequence::iterator it = features_.begin();
		it != features_.end();
		++it)
	{
		if(!it->optional_key_code())
			continue;

		if(*(it->optional_key_code()) == e.key_code())
		{
			it->is_active(
				!it->is_active());
			this->post_notification(
				notifications::text(
					it->json_identifier().get()+
					FCPPT_TEXT(" ")+
					bool_to_string(
						it->is_active())));
			break;
		}
	}
}