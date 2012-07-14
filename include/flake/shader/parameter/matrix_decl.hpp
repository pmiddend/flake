#ifndef FLAKE_SHADER_PARAMETER_MATRIX_DECL_HPP_INCLUDED
#define FLAKE_SHADER_PARAMETER_MATRIX_DECL_HPP_INCLUDED

#include <flake/shader/parameter/matrix_fwd.hpp>
#include <flake/shader/parameter/is_projection_matrix.hpp>
#include <flake/shader/parameter/name.hpp>
#include <sge/cg/program/object_fwd.hpp>
#include <sge/cg/parameter/named.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/math/matrix/static.hpp>

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
class matrix
{
FCPPT_NONCOPYABLE(
	matrix);
public:
	typedef
	ValueType
	value_type;

	typedef typename
	fcppt::math::matrix::static_<value_type,M,N>::type
	matrix_type;

	static fcppt::math::size_type const rows =
		M;

	static fcppt::math::size_type const columns =
		N;

	matrix(
		sge::cg::program::object &,
		flake::shader::parameter::name const &,
		flake::shader::parameter::is_projection_matrix const &,
		matrix_type const &);

	void
	set(
		matrix_type const &);

	~matrix();
private:
	sge::cg::parameter::named const parameter_;
	flake::shader::parameter::is_projection_matrix const is_projection_matrix_;
};
}
}
}

#endif
