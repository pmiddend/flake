#ifndef FLAKELIB_VOLUME_BOUNDARY_OBSTACLE_SEQUENCE_HPP_INCLUDED
#define FLAKELIB_VOLUME_BOUNDARY_OBSTACLE_SEQUENCE_HPP_INCLUDED

#include <flakelib/volume/boundary/obstacle_variant.hpp>
#include <fcppt/config/external_begin.hpp>
#include <vector>
#include <fcppt/config/external_end.hpp>


namespace flakelib
{
namespace volume
{
namespace boundary
{
typedef
std::vector<boundary::obstacle_variant>
obstacle_sequence;
}
}
}

#endif
