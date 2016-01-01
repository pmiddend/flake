#include <flake/test/camera_manager.hpp>
#include <sge/camera/coordinate_system/identity.hpp>
#include <sge/camera/first_person/object.hpp>
#include <sge/camera/first_person/parameters.hpp>
#include <sge/camera/tracking/object.hpp>
#include <sge/camera/tracking/json/interval_exporter.hpp>
#include <sge/camera/tracking/json/keyframes_from_json.hpp>
#include <sge/input/keyboard/device.hpp>
#include <sge/input/keyboard/key_callback.hpp>
#include <sge/input/keyboard/key_event.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <sge/parse/json/is_null.hpp>
#include <sge/parse/json/parse_file_exn.hpp>
#include <sge/parse/json/path.hpp>
#include <sge/parse/json/value.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/optional/maybe_void.hpp>
#include <fcppt/string.hpp>
#include <fcppt/text.hpp>
#include <fcppt/unique_ptr_to_base.hpp>
#include <fcppt/math/deg_to_rad.hpp>
#include <fcppt/signal/connection.hpp>
#include <fcppt/config/external_begin.hpp>
#include <functional>
#include <memory>
#include <fcppt/config/external_end.hpp>


flake::test::camera_manager::camera_manager(
	sge::parse::json::object const &_object,
	sge::input::keyboard::device &_keyboard,
	sge::input::mouse::device &_mouse,
	sge::viewport::manager &_viewport_manager)
:
	camera_(
		sge::parse::json::is_null(
			sge::parse::json::find_and_convert_member<sge::parse::json::value>(
				_object,
				sge::parse::json::path(
					FCPPT_TEXT("keyframe-import-path"))))
		?
			fcppt::unique_ptr_to_base<
				sge::camera::base
			>(
				fcppt::make_unique_ptr<sge::camera::first_person::object>(
					sge::camera::first_person::parameters(
						_keyboard,
						_mouse,
						sge::camera::is_active(
							true),
						sge::camera::first_person::movement_speed(
							sge::parse::json::find_and_convert_member<sge::renderer::scalar>(
								_object,
								sge::parse::json::path(
									FCPPT_TEXT("camera-movement-speed")))),
						sge::camera::coordinate_system::identity())))
		:
			fcppt::unique_ptr_to_base<
				sge::camera::base
			>(
				fcppt::make_unique_ptr<sge::camera::tracking::object>(
					sge::camera::optional_projection_matrix(),
					sge::camera::tracking::json::keyframes_from_json(
						sge::parse::json::parse_file_exn(
							boost::filesystem::path(
								sge::parse::json::find_and_convert_member<fcppt::string>(
									_object,
									sge::parse::json::path(
										FCPPT_TEXT("keyframe-import-path"))))).array()),
					sge::camera::tracking::is_looping(
						sge::parse::json::find_and_convert_member<bool>(
							_object,
							sge::parse::json::path(
								FCPPT_TEXT("loop-keyframes")))),
					sge::camera::is_active(
						true)))),
	perspective_projection_from_viewport_(
		dynamic_cast<sge::camera::has_mutable_projection &>(
			*camera_),
		_viewport_manager,
		sge::renderer::projection::near(
			sge::parse::json::find_and_convert_member<sge::renderer::scalar>(
				_object,
				sge::parse::json::path(
					FCPPT_TEXT("near-plane")))),
		sge::renderer::projection::far(
			sge::parse::json::find_and_convert_member<sge::renderer::scalar>(
				_object,
				sge::parse::json::path(
					FCPPT_TEXT("far-plane")))),
		sge::renderer::projection::fov(
			fcppt::math::deg_to_rad(
				sge::parse::json::find_and_convert_member<sge::renderer::scalar>(
					_object,
					sge::parse::json::path(
						FCPPT_TEXT("fov")))))),
	exporter_(
		sge::parse::json::is_null(
			sge::parse::json::find_and_convert_member<sge::parse::json::value>(
				_object,
				sge::parse::json::path(
					FCPPT_TEXT("keyframe-export-path"))))
		?
			optional_interval_exporter()
		:
			optional_interval_exporter(
				fcppt::make_unique_ptr<
					sge::camera::tracking::json::interval_exporter
				>(
					*camera_,
					sge::camera::update_duration(
						sge::parse::json::find_and_convert_member<
							sge::camera::update_duration::rep
						>(
							_object,
							sge::parse::json::path(
								FCPPT_TEXT("keyframe-interval")
							)
						)
					),
					boost::filesystem::path(
						sge::parse::json::find_and_convert_member<
							fcppt::string
						>(
							_object,
							sge::parse::json::path(
								FCPPT_TEXT("keyframe-export-path")
							)
						)
					)
				)
			)
	),
	key_press_connection_(
		_keyboard.key_callback(
			sge::input::keyboard::key_callback{
				std::bind(
					&camera_manager::key_press_callback,
					this,
					std::placeholders::_1
				)
			}
		)
	),
	recording_(
		false)
{
}

void
flake::test::camera_manager::update(
	sge::camera::update_duration const &_duration)
{
	if
		(recording_
	)
		fcppt::optional::maybe_void(
			exporter_,
			[](
				auto const &_exporter
			)
			{
				_exporter->update();
			}
		);

	dynamic_cast<sge::camera::is_dynamic &>(
		*camera_).update(
		_duration);
}

sge::camera::base &
flake::test::camera_manager::camera()
{
	return
		*camera_;
}

flake::test::camera_manager::~camera_manager()
{
}

void
flake::test::camera_manager::key_press_callback(
	sge::input::keyboard::key_event const &_key_event)
{
	if(!_key_event.pressed())
		return;

	if(_key_event.key().code() == sge::input::key::code::f8)
		recording_ =
			!recording_;
}
