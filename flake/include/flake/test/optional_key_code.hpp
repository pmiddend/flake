#ifndef FLAKE_TEST_OPTIONAL_KEY_CODE_HPP_INCLUDED
#define FLAKE_TEST_OPTIONAL_KEY_CODE_HPP_INCLUDED

#include <sge/input/keyboard/key_code.hpp>
#include <fcppt/optional_fwd.hpp>

namespace flake
{
namespace test
{
typedef
fcppt::optional<sge::input::keyboard::key_code::type>
optional_key_code;
}
}

#endif

