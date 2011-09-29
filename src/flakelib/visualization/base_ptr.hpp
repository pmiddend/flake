#ifndef FLAKELIB_VISUALIZATION_BASE_PTR_HPP_INCLUDED
#define FLAKELIB_VISUALIZATION_BASE_PTR_HPP_INCLUDED

#include <flakelib/visualization/base_fwd.hpp>
#include <fcppt/unique_ptr.hpp>

namespace flakelib
{
namespace visualization
{
typedef
fcppt::unique_ptr<visualization::base>
base_ptr;
}
}

#endif
