#include <flakelib/volume/boundary/json/to_cube.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <sge/parse/json/path.hpp>
#include <fcppt/text.hpp>

flakelib::volume::boundary::cube::object const
flakelib::volume::boundary::json::to_cube(
	sge::parse::json::object const &_object)
{
	return
		boundary::cube::object(
			boundary::cube::size(
				sge::parse::json::find_and_convert_member<sge::opencl::memory_object::dim3>(
					_object,
					sge::parse::json::path(
						FCPPT_TEXT("size")))),
			boundary::cube::position(
				sge::parse::json::find_and_convert_member<sge::opencl::memory_object::dim3>(
					_object,
					sge::parse::json::path(
						FCPPT_TEXT("position")))));
}
