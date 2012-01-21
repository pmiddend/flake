#include <flakelib/volume/boundary/json/to_cube.hpp>
#include <flakelib/volume/boundary/json/to_obstacle_variant.hpp>
#include <flakelib/volume/boundary/json/to_sphere.hpp>
#include <sge/parse/json/array.hpp>
#include <sge/parse/json/find_member_exn.hpp>
#include <sge/parse/json/object.hpp>
#include <sge/parse/json/value.hpp>
#include <fcppt/string.hpp>
#include <fcppt/text.hpp>
#include <fcppt/assert/unreachable.hpp>


flakelib::volume::boundary::obstacle_variant const
flakelib::volume::boundary::json::to_obstacle_variant(
	sge::parse::json::object const &_object)
{
	fcppt::string const type =
		sge::parse::json::find_member_exn<fcppt::string>(
			_object.members,
			FCPPT_TEXT("type"));

	if(type == FCPPT_TEXT("cube"))
	{
		return
			boundary::obstacle_variant(
				json::to_cube(
					_object));
	}
	else if(type == FCPPT_TEXT("sphere"))
	{
		return
			boundary::obstacle_variant(
				json::to_sphere(
					_object));
	}

	FCPPT_ASSERT_UNREACHABLE;
}
