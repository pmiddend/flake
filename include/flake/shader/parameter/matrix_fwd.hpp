#ifndef FLAKE_SHADER_PARAMETER_MATRIX_FWD_HPP_INCLUDED
#define FLAKE_SHADER_PARAMETER_MATRIX_FWD_HPP_INCLUDED

#include <fcppt/math/size_type.hpp>

namespace flake
{
namespace shader
{
namespace parameter
{
template
<
	typename ValueType,
	fcppt::math::size_type M,
	fcppt::math::size_type N
>
class matrix;
}
}
}

#endif
