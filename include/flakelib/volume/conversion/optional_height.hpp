#ifndef FLAKELIB_VOLUME_CONVERSION_OPTIONAL_HEIGHT_HPP_INCLUDED
#define FLAKELIB_VOLUME_CONVERSION_OPTIONAL_HEIGHT_HPP_INCLUDED

#include <fcppt/optional_fwd.hpp>
#include <sge/opencl/size_type.hpp>

namespace flakelib
{
namespace volume
{
namespace conversion
{
typedef
fcppt::optional<sge::opencl::size_type>
optional_height;
}
}
}

#endif
