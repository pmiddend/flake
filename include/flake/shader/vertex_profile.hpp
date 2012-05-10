#ifndef FLAKE_SHADER_VERTEX_PROFILE_HPP_INCLUDED
#define FLAKE_SHADER_VERTEX_PROFILE_HPP_INCLUDED

#include <fcppt/strong_typedef.hpp>
#include <sge/cg/profile/object_fwd.hpp>

namespace flake
{
namespace shader
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::cg::profile::object const &,
	vertex_profile);
}
}

#endif

