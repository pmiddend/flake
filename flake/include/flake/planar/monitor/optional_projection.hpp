#ifndef FLAKE_PLANAR_MONITOR_OPTIONAL_PROJECTION_HPP_INCLUDED
#define FLAKE_PLANAR_MONITOR_OPTIONAL_PROJECTION_HPP_INCLUDED

#include <sge/renderer/matrix4.hpp>
#include <fcppt/math/matrix/object_impl.hpp>
#include <fcppt/optional/object_fwd.hpp>


namespace flake
{
namespace planar
{
namespace monitor
{
typedef
fcppt::optional::object<sge::renderer::matrix4>
optional_projection;
}
}
}

#endif
