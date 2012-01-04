#ifndef FLAKELIB_PLANAR_MONITOR_OPTIONAL_PROJECTION_HPP_INCLUDED
#define FLAKELIB_PLANAR_MONITOR_OPTIONAL_PROJECTION_HPP_INCLUDED

#include <sge/renderer/matrix4.hpp>
#include <fcppt/optional_fwd.hpp>
#include <fcppt/math/matrix/basic_impl.hpp>


namespace flakelib
{
namespace planar
{
namespace monitor
{
typedef
fcppt::optional<sge::renderer::matrix4>
optional_projection;
}
}
}

#endif
