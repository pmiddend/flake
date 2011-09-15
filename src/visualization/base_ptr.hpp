#ifndef FLAKE_VISUALIZATION_BASE_PTR_HPP_INCLUDED
#define FLAKE_VISUALIZATION_BASE_PTR_HPP_INCLUDED

#include "base_fwd.hpp"
#include <fcppt/unique_ptr.hpp>

namespace flake
{
namespace visualization
{
typedef
fcppt::unique_ptr<visualization::base>
base_ptr;
}
}

#endif
