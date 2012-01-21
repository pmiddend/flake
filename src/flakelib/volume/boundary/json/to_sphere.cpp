#include <flakelib/volume/boundary/json/to_sphere.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <sge/parse/json/path.hpp>
#include <fcppt/text.hpp>

flakelib::volume::boundary::sphere::object const
flakelib::volume::boundary::json::to_sphere(
	sge::parse::json::object const &_object)
{
	return
		boundary::sphere::object(
			boundary::sphere::radius(
				sge::parse::json::find_and_convert_member<sge::opencl::memory_object::size_type>(
					_object,
					sge::parse::json::path(
						FCPPT_TEXT("radius")))),
			boundary::sphere::position(
				sge::parse::json::find_and_convert_member<sge::opencl::memory_object::dim3>(
					_object,
					sge::parse::json::path(
						FCPPT_TEXT("center")))));
}
