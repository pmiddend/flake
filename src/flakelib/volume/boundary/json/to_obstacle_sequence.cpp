#include <flakelib/volume/boundary/json/to_obstacle_sequence.hpp>
#include <flakelib/volume/boundary/json/to_obstacle_variant.hpp>
#include <sge/parse/json/array.hpp>
#include <sge/parse/json/get.hpp>
#include <sge/parse/json/object.hpp>

flakelib::volume::boundary::obstacle_sequence const
flakelib::volume::boundary::json::to_obstacle_sequence(
	sge::parse::json::array const &a)
{
	flakelib::volume::boundary::obstacle_sequence result;

	for(
		sge::parse::json::element_vector::const_iterator it =
			a.elements.begin();
		it != a.elements.end();
		++it)
	{
		result.push_back(
			json::to_obstacle_variant(
				sge::parse::json::get<sge::parse::json::object>(
					*it)));
	}

	return result;
}
