#ifndef FLAKELIB_VALUE_MODULATOR_VALUE_MODIFY_CALLBACK_HPP_INCLUDED
#define FLAKELIB_VALUE_MODULATOR_VALUE_MODIFY_CALLBACK_HPP_INCLUDED

#include <flakelib/value_modulator/value_modify_callback_fn.hpp>
#include <fcppt/config/external_begin.hpp>
#include <functional>
#include <fcppt/config/external_end.hpp>

namespace flakelib
{
namespace value_modulator
{
typedef
std::function<value_modulator::value_modify_callback_fn>
value_modify_callback;
}
}

#endif

