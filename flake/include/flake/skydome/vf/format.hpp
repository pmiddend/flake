#ifndef FLAKE_SKYDOME_VF_FORMAT_HPP_INCLUDED
#define FLAKE_SKYDOME_VF_FORMAT_HPP_INCLUDED

#include <flake/skydome/vf/format_part.hpp>
#include <sge/renderer/vf/format.hpp>
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
sge::renderer::vf::format
<
	boost::mpl::vector1<flake::skydome::vf::format_part>
>
format;
}
}
}

#endif

