#ifndef FLAKELIB_PLANAR_SIMULATION_BASE_PTR_HPP_INCLUDED
#define FLAKELIB_PLANAR_SIMULATION_BASE_PTR_HPP_INCLUDED

#include <flakelib/planar/simulation/base_fwd.hpp>
#include <fcppt/unique_ptr.hpp>

namespace flakelib
{
namespace planar
{
namespace simulation
{
typedef
fcppt::unique_ptr<simulation::base>
base_ptr;
}
}
}

#endif
