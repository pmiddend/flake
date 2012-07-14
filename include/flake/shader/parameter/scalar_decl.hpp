#ifndef FLAKE_SHADER_PARAMETER_SCALAR_DECL_HPP_INCLUDED
#define FLAKE_SHADER_PARAMETER_SCALAR_DECL_HPP_INCLUDED

#include <flake/shader/parameter/scalar_fwd.hpp>
#include <sge/cg/program/object_fwd.hpp>
#include <fcppt/noncopyable.hpp>

namespace flake
{
namespace shader
{
namespace parameter
{
template<typename ValueType>
class scalar
{
FCPPT_NONCOPYABLE(
	scalar);
public:
	typedef
	ValueType
	value_type;

	scalar(
		sge::cg::program::object &,
		flake::shader::parameter::name const &,
		value_type);

	void
	set(
		value_type);

	~scalar();
private:
	sge::cg::parameter::named const parameter_;
};
}
}
}

#endif
