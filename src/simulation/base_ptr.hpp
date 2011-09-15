#ifndef FLAKE_SIMULATION_BASE_PTR_HPP_INCLUDED
#define FLAKE_SIMULATION_BASE_PTR_HPP_INCLUDED

#include "base_fwd.hpp"
#include <fcppt/unique_ptr.hpp>

namespace flake
{
namespace simulation
{
typedef
fcppt::unique_ptr<base>
base_ptr;
}
}

#endif
