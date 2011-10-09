#ifndef FLAKELIB_SIMULATION_BASE_PTR_HPP_INCLUDED
#define FLAKELIB_SIMULATION_BASE_PTR_HPP_INCLUDED

#include <flakelib/simulation/base_fwd.hpp>
#include <fcppt/unique_ptr.hpp>

namespace flakelib
{
namespace simulation
{
typedef
fcppt::unique_ptr<base>
base_ptr;
}
}

#endif
