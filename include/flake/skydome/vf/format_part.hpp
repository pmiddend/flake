#ifndef FLAKE_SKYDOME_VF_FORMAT_PART_HPP_INCLUDED
#define FLAKE_SKYDOME_VF_FORMAT_PART_HPP_INCLUDED

#include <flake/skydome/vf/position.hpp>
#include <flake/skydome/vf/texcoord.hpp>
#include <sge/renderer/vf/part.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/mpl/vector/vector10.hpp>
#include <fcppt/config/external_end.hpp>


namespace flake
{
namespace skydome
{
namespace vf
{
typedef
sge::renderer::vf::part
<
	boost::mpl::vector1
	<
		flake::skydome::vf::position/*,
									  flake::skydome::vf::texcoord*/
	>
>
format_part;
}
}
}

#endif

