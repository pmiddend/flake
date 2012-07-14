#ifndef FLAKE_SHADER_PARAMETER_NAME_HPP_INCLUDED
#define FLAKE_SHADER_PARAMETER_NAME_HPP_INCLUDED

#include <fcppt/strong_typedef.hpp>
#include <sge/cg/string.hpp>

namespace flake
{
namespace shader
{
namespace parameter
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::cg::string,
	name);
}
}
}

#endif
