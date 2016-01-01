#ifndef FLAKELIB_VOLUME_CONVERSION_OPTIONAL_HEIGHT_HPP_INCLUDED
#define FLAKELIB_VOLUME_CONVERSION_OPTIONAL_HEIGHT_HPP_INCLUDED

#include <sge/opencl/size_type.hpp>
#include <fcppt/optional/object_fwd.hpp>


namespace flakelib
{
namespace volume
{
namespace conversion
{
typedef
fcppt::optional::object<sge::opencl::size_type>
optional_height;
}
}
}

#endif
