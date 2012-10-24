#ifndef FLAKE_POSTPROCESSING_VF_FORMAT_PART_HPP_INCLUDED
#define FLAKE_POSTPROCESSING_VF_FORMAT_PART_HPP_INCLUDED

#include <flake/postprocessing/vf/position.hpp>
#include <flake/postprocessing/vf/texcoord.hpp>
#include <sge/renderer/vf/part.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/mpl/vector/vector10.hpp>
#include <fcppt/config/external_end.hpp>


namespace flake
{
namespace postprocessing
{
namespace vf
{
typedef
sge::renderer::vf::part
<
	boost::mpl::vector2
	<
		flake::postprocessing::vf::position,
		flake::postprocessing::vf::texcoord
	>
>
format_part;
}
}
}

#endif

