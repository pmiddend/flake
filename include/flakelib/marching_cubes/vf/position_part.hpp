#ifndef FLAKELIB_MARCHING_CUBES_VF_POSITION_PART_HPP_INCLUDED
#define FLAKELIB_MARCHING_CUBES_VF_POSITION_PART_HPP_INCLUDED

#include <flakelib/marching_cubes/vf/position.hpp>
#include <sge/renderer/vf/part.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/mpl/vector/vector10.hpp>
#include <fcppt/config/external_end.hpp>


namespace flakelib
{
namespace marching_cubes
{
namespace vf
{
typedef
sge::renderer::vf::part
<
	boost::mpl::vector1
	<
		vf::position
	>
>
position_part;
}
}
}

#endif

