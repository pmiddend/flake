#ifndef FLAKE_POSTPROCESSING_VF_FORMAT_HPP_INCLUDED
#define FLAKE_POSTPROCESSING_VF_FORMAT_HPP_INCLUDED

#include <flake/postprocessing/vf/format_part.hpp>
#include <sge/renderer/vf/format.hpp>
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
sge::renderer::vf::format
<
	boost::mpl::vector1<flake::postprocessing::vf::format_part>
>
format;
}
}
}

#endif

