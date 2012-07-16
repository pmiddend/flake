#ifndef FLAKE_TEST_BASE_HPP_INCLUDED
#define FLAKE_TEST_BASE_HPP_INCLUDED

#include <flake/font_metrics_cache.hpp>
#include <flake/notifications/object_fwd.hpp>
#include <flake/notifications/text.hpp>
#include <flake/test/feature_sequence.hpp>
#include <flake/postprocessing/context.hpp>
#include <flake/test/json_identifier.hpp>
#include <flake/test/information/manager_fwd.hpp>
#include <flake/test/information/object.hpp>
#include <flake/time_modifier/multiplier.hpp>
#include <flake/time_modifier/object_fwd.hpp>
#include <flake/scoped_ostream_file_redirection.hpp>
#include <flake/scoped_wostream_file_redirection.hpp>
#include <flake/shader/context_fwd.hpp>
#include <flakelib/scoped_frame_limiter.hpp>
#include <flakelib/buffer_pool/object_fwd.hpp>
#include <flakelib/cl/program_context_fwd.hpp>
#include <sge/charconv/system_fwd.hpp>
#include <sge/font/system_fwd.hpp>
#include <sge/image2d/system_fwd.hpp>
#include <sge/input/cursor/object_fwd.hpp>
#include <sge/input/keyboard/device_fwd.hpp>
#include <sge/input/keyboard/key_event_fwd.hpp>
#include <sge/input/mouse/device_fwd.hpp>
#include <sge/opencl/single_device_system/object_fwd.hpp>
#include <sge/parse/json/object.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/renderer/context/object_fwd.hpp>
#include <sge/systems/cursor_option_field.hpp>
#include <sge/systems/instance_fwd.hpp>
#include <sge/viewport/manager_fwd.hpp>
#include <sge/window/title.hpp>
#include <awl/main/exit_code.hpp>
#include <awl/main/function_context_fwd.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/scoped_ptr.hpp>
#include <fcppt/config/platform.hpp>
#include <fcppt/signal/scoped_connection.hpp>
#include <fcppt/signal/scoped_connection.hpp>


namespace flake
{
namespace test
{
class base
{
FCPPT_NONCOPYABLE(
	base);
public:
	awl::main::exit_code const
	run();

	virtual ~base() = 0;
protected:
	base(
		awl::main::function_context const &,
		sge::window::title const &,
		test::json_identifier const &,
		test::feature_sequence const &,
		sge::systems::cursor_option_field const &);

	virtual void
	update() = 0;

	virtual void
	render(
		sge::renderer::context::object &) = 0;

	virtual void
	render_overlay(
		sge::renderer::context::object &);

	sge::parse::json::object const &
	configuration() const;

	sge::renderer::device &
	renderer();

	flake::shader::context &
	shader_context();

	sge::opencl::single_device_system::object &
	opencl_system();

	sge::input::cursor::object &
	cursor();

	sge::input::mouse::device &
	mouse();

	sge::input::keyboard::device &
	keyboard();

	sge::font::system &
	font_system();

	flakelib::cl::program_context const &
	program_context() const;

	sge::image2d::system &
	image_system();

	flakelib::buffer_pool::object &
	buffer_pool();

	sge::viewport::manager &
	viewport_manager();

	void
	post_notification(
		notifications::text const &);

	time_modifier::multiplier const
	current_multiplier() const;

	bool
	feature_active(
		test::json_identifier const &) const;

	flake::test::information::manager &
	information_manager();

	bool
	dump_this_frame() const;
protected:
	virtual void
	viewport_callback();
private:
#ifdef FCPPT_CONFIG_WINDOWS_PLATFORM
	flake::scoped_ostream_file_redirection error_redirection_;
	flake::scoped_ostream_file_redirection log_redirection_;
	flake::scoped_ostream_file_redirection output_redirection_;
	flake::scoped_wostream_file_redirection werror_redirection_;
	flake::scoped_wostream_file_redirection wlog_redirection_;
	flake::scoped_wostream_file_redirection woutput_redirection_;
#endif
	fcppt::scoped_ptr<sge::charconv::system> charconv_system_;
	fcppt::scoped_ptr<sge::parse::json::object> configuration_;
	sge::parse::json::object const local_configuration_;
	test::feature_sequence features_;
	fcppt::scoped_ptr<sge::systems::instance> systems_;
	fcppt::signal::scoped_connection quit_connection_;
	fcppt::scoped_ptr<flake::shader::context> shader_context_;
	fcppt::scoped_ptr<sge::opencl::single_device_system::object> opencl_system_;
	fcppt::scoped_ptr<flakelib::cl::program_context> program_context_;
	fcppt::scoped_ptr<flakelib::buffer_pool::object> buffer_pool_;
	flakelib::scoped_frame_limiter::fps_type desired_fps_;
	fcppt::signal::scoped_connection viewport_connection_;
	fcppt::scoped_ptr<flake::font_metrics_cache> font_metrics_cache_;
	fcppt::scoped_ptr<flake::notifications::object> notifications_;
	fcppt::scoped_ptr<flake::test::information::manager> information_manager_;
	flake::test::information::object memory_consumption_information_;
	fcppt::scoped_ptr<flake::time_modifier::object> time_modifier_;
	fcppt::signal::scoped_connection key_callback_connection_;
	flake::postprocessing::context postprocessing_;
	bool dump_this_frame_;

	void
	key_callback(
		sge::input::keyboard::key_event const &);
};
}
}

#endif

