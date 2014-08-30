#include <flake/planar/monitor/font_axis_policy.hpp>
#include <sge/font/dim.hpp>
#include <sge/font/object.hpp>
#include <sge/font/text.hpp>
#include <fcppt/math/box/object_impl.hpp>
#include <fcppt/math/dim/object_impl.hpp>


sge::rucksack::axis_policy2 const
flake::planar::monitor::font_axis_policy(
	sge::font::object &_font,
	sge::font::text_parameters const &_parameters,
	sge::font::string const &_text)
{
	sge::font::dim const size =
		_font.create_text(
			_text,
			_parameters)->rect().size();

	return
		sge::rucksack::axis_policy2(
			sge::rucksack::axis_policy(
				sge::rucksack::preferred_size{
					static_cast<sge::rucksack::scalar>(
						size.w())}),
			sge::rucksack::axis_policy(
				sge::rucksack::preferred_size{
					static_cast<sge::rucksack::scalar>(
						size.h())}));
}
