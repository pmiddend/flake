#ifndef FLAKELIB_VOLUME_DENSITY_CUBE_VF_FORMAT_HPP_INCLUDED
#define FLAKELIB_VOLUME_DENSITY_CUBE_VF_FORMAT_HPP_INCLUDED

#include <flakelib/volume/density/cube_vf/format_part.hpp>
#include <sge/renderer/vf/format.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/mpl/vector/vector10.hpp>
#include <fcppt/config/external_end.hpp>


namespace flakelib
{
namespace volume
{
namespace density
{
namespace cube_vf
{
typedef
sge::renderer::vf::format
<
	boost::mpl::vector1<cube_vf::format_part>
>
format;
}
}
}
}

#endif
