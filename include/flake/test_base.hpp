#ifndef FLAKE_TEST_BASE_HPP_INCLUDED
#define FLAKE_TEST_BASE_HPP_INCLUDED

#include <flake/notifications/object_fwd.hpp>
#include <flake/notifications/text.hpp>
#include <flake/time_modifier/multiplier.hpp>
#include <flake/time_modifier/object_fwd.hpp>
#include <flakelib/scoped_frame_limiter.hpp>
#include <flakelib/buffer_pool/object_fwd.hpp>
#include <flakelib/cl/program_context_fwd.hpp>
#include <sge/charconv/system_fwd.hpp>
#include <sge/font/system_fwd.hpp>
#include <sge/image2d/system_fwd.hpp>
#include <sge/input/cursor/object_fwd.hpp>
#include <sge/input/keyboard/device_fwd.hpp>
#include <sge/input/mouse/device_fwd.hpp>
#include <sge/opencl/single_device_system/object_fwd.hpp>
#include <sge/parse/json/object_fwd.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/systems/cursor_option_field.hpp>
#include <sge/systems/instance_fwd.hpp>
#include <sge/viewport/manager_fwd.hpp>
#include <sge/window/title.hpp>
#include <awl/main/exit_code.hpp>
#include <awl/main/function_context_fwd.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/scoped_ptr.hpp>
#include <fcppt/signal/scoped_connection.hpp>


namespace flake
{
class test_base
{
FCPPT_NONCOPYABLE(
	test_base);
public:
	awl::main::exit_code const
	run();

	virtual ~test_base() = 0;
protected:
	explicit
	test_base(
		awl::main::function_context const &,
		sge::window::title const &,
		sge::systems::cursor_option_field const &);

	virtual void
	update() = 0;

	virtual void
	render() = 0;

	sge::parse::json::object const &
	configuration() const;

	sge::renderer::device &
	renderer();

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
protected:
	virtual void
	viewport_callback();
private:
	fcppt::scoped_ptr<sge::charconv::system> charconv_system_;
	fcppt::scoped_ptr<sge::parse::json::object> configuration_;
	fcppt::scoped_ptr<sge::systems::instance> systems_;
	fcppt::scoped_ptr<sge::opencl::single_device_system::object> opencl_system_;
	fcppt::scoped_ptr<flakelib::cl::program_context> program_context_;
	fcppt::scoped_ptr<flakelib::buffer_pool::object> buffer_pool_;
	flakelib::scoped_frame_limiter::fps_type desired_fps_;
	fcppt::signal::scoped_connection viewport_connection_;
	fcppt::scoped_ptr<flake::notifications::object> notifications_;
	fcppt::scoped_ptr<flake::time_modifier::object> time_modifier_;
};
}

#endif

