#ifndef FLAKELIB_VALUE_MODULATOR_VALUE_MODIFY_CALLBACK_HPP_INCLUDED
#define FLAKELIB_VALUE_MODULATOR_VALUE_MODIFY_CALLBACK_HPP_INCLUDED

#include <flakelib/value_modulator/value_modify_callback_fn.hpp>
#include <fcppt/function/object.hpp>

namespace flakelib
{
namespace value_modulator
{
typedef
fcppt::function::object<value_modulator::value_modify_callback_fn>
value_modify_callback;
}
}

#endif

