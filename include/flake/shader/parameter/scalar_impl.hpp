#ifndef FLAKE_SHADER_PARAMETER_SCALAR_IMPL_HPP_INCLUDED
#define FLAKE_SHADER_PARAMETER_SCALAR_IMPL_HPP_INCLUDED

#include <flake/shader/parameter/scalar_decl.hpp>
#include <sge/cg/program/object.hpp>
#include <sge/cg/parameter/scalar/set.hpp>

template<typename ValueType>
flake::shader::parameter::scalar<ValueType>::scalar(
	sge::cg::program::object &_program,
	flake::shader::parameter::name const &_name,
	value_type const _initial_value)
:
	parameter_(
		_program.parameter(
			_name.get()))
{
	this->set(
		_initial_value);
}

template<typename ValueType>
void
flake::shader::parameter::scalar<ValueType>::set(
	value_type const _scalar)
{
	sge::cg::parameter::scalar::set(
		parameter_.object(),
		_scalar);
}

template<typename ValueType>
flake::shader::parameter::scalar<ValueType>::~scalar()
{
}

#endif
