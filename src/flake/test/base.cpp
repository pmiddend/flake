#include <flake/media_path_from_string.hpp>
#include <sge/image/colors.hpp>
#include <sge/image/color/any/object.hpp>
#include <flake/notifications/object.hpp>
#include <flake/test/base.hpp>
#include <flake/test/update_features_from_json.hpp>
#include <flake/test/information/manager.hpp>
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
#include <sge/font/system.hpp>
#include <sge/image/colors.hpp>
#include <sge/input/keyboard/device.hpp>
#include <sge/input/keyboard/key_event.hpp>
#include <sge/log/global_context.hpp>
#include <sge/media/all_extensions.hpp>
#include <sge/opencl/single_device_system/object.hpp>
#include <sge/opencl/single_device_system/parameters.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <sge/parse/json/parse_string_exn.hpp>
#include <sge/parse/json/string_to_path.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/context/scoped.hpp>
#include <sge/renderer/context/scoped_scoped_ptr.hpp>
#include <sge/renderer/parameters/object.hpp>
#include <sge/renderer/pixel_format/object.hpp>
#include <sge/renderer/target/onscreen.hpp>
#include <sge/shader/context.hpp>
#include <sge/systems/font.hpp>
#include <sge/systems/image2d.hpp>
#include <sge/systems/input.hpp>
#include <sge/systems/instance.hpp>
#include <sge/systems/list.hpp>
#include <sge/systems/quit_on_escape.hpp>
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
#include <fcppt/format.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/optional_impl.hpp>
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
#include <fcppt/signal/connection.hpp>
#include <fcppt/tr1/functional.hpp>
#include <fcppt/config/external_begin.hpp>
#include <exception>
#include <iostream>
#include <ostream>
#include <fcppt/config/external_end.hpp>


namespace
{
fcppt::string
byte_size_to_megabytes(
	sge::opencl::memory_object::byte_size const &b)
{
	return
		(fcppt::format(
			FCPPT_TEXT("%.2fMiB")) %
		 (static_cast<float>(b.get()) / 1024.f / 1024.f)).str();
}
}

awl::main::exit_code const
flake::test::base::run()
{
	systems_->window().show();

	while(systems_->window_system().poll())
	{
		flakelib::scoped_frame_limiter frame_limiter(
			desired_fps_);

		this->update();

		if(this->renderer().onscreen_target().viewport().get().content())
		{
			this->render(
				postprocessing_.create_render_context()->get());

			this->render_overlay(
				postprocessing_.render_and_return_overlay()->get());
		}

		dump_this_frame_ =
			false;
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
#ifdef FCPPT_CONFIG_WINDOWS_PLATFORM
	error_redirection_(
		std::cerr,
		"error.txt"),
	log_redirection_(
		std::clog,
		"log.txt"),
	output_redirection_(
		std::cout,
		"output.txt"),
	werror_redirection_(
		std::wcerr,
		"werror.txt"),
	wlog_redirection_(
		std::wclog,
		"wlog.txt"),
	woutput_redirection_(
		std::wcout,
		"woutput.txt"),
#endif
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
							sge::renderer::parameters::object(
								sge::renderer::pixel_format::object(
									sge::renderer::pixel_format::color::depth32,
									sge::renderer::pixel_format::depth_stencil::off,
									sge::renderer::pixel_format::optional_multi_samples(),
									sge::renderer::pixel_format::srgb::no),
								sge::renderer::parameters::vsync::on,
								sge::renderer::display_mode::optional_object()),
							sge::viewport::fill_on_resize())
						 	.caps(
								sge::renderer::caps::system_field(
									sge::renderer::caps::system::opengl)))
						(sge::systems::font())
						(sge::systems::input(
							sge::systems::input_helper_field(
								sge::systems::input_helper::keyboard_collector)
								| sge::systems::input_helper::cursor_demuxer
								| sge::systems::input_helper::mouse_collector,
							_cursor_options))))),
	quit_connection_(
		sge::systems::quit_on_escape(
			*systems_)),
	shader_context_(
		fcppt::make_unique_ptr<sge::shader::context>(
			fcppt::ref(
				this->renderer()))),
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
			fcppt::ref(
				this->font_system()),
			sge::parse::json::find_and_convert_member<sge::font::ttf_size>(
				*configuration_,
				sge::parse::json::string_to_path(
					FCPPT_TEXT("tests/notification-font-size"))),
			flake::notifications::time_to_live(
				boost::chrono::milliseconds(
					sge::parse::json::find_and_convert_member<boost::chrono::milliseconds::rep>(
						*configuration_,
						sge::parse::json::string_to_path(
							FCPPT_TEXT("tests/notification-ttl-ms"))))))),
	information_manager_(
		fcppt::make_unique_ptr<flake::test::information::manager>(
			fcppt::ref(
				this->font_system()),
			sge::parse::json::find_and_convert_member<sge::font::ttf_size>(
				*configuration_,
				sge::parse::json::string_to_path(
					FCPPT_TEXT("tests/information-font-size"))),
			fcppt::ref(
				this->renderer()),
			sge::image::colors::white())),
	memory_consumption_information_(
		*information_manager_,
		information::identifier(
			FCPPT_TEXT("CL memory")),
		std::tr1::bind(
			&byte_size_to_megabytes,
			std::tr1::bind(
				&flakelib::buffer_pool::object::memory_consumption,
				buffer_pool_.get()))),
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
				std::tr1::placeholders::_1))),
	postprocessing_(
		this->renderer(),
		this->viewport_manager(),
		this->shader_context()),
	dump_this_frame_(
		false)
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
flake::test::base::render_overlay(
	sge::renderer::context::object &_context)
{
	notifications_->render(
		_context);
	information_manager_->render(
		_context);
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

sge::shader::context &
flake::test::base::shader_context()
{
	return
		*shader_context_;
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

flake::test::information::manager &
flake::test::base::information_manager()
{
	return
		*information_manager_;
}

bool
flake::test::base::dump_this_frame() const
{
	return
		dump_this_frame_;
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

	if(e.key_code() == sge::input::keyboard::key_code::sysrq)
	{
		dump_this_frame_ =
			true;
		this->post_notification(
			notifications::text(
				FCPPT_TEXT("Dumped status...")));
		return;
	}

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
