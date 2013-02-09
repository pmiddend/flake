#ifndef FLAKE_TEST_INFORMATION_PULL_FUNCTION_HPP_INCLUDED
#define FLAKE_TEST_INFORMATION_PULL_FUNCTION_HPP_INCLUDED

#include <flake/test/information/pull_function_fn.hpp>
#include <fcppt/config/external_begin.hpp>
#include <functional>
#include <fcppt/config/external_end.hpp>

namespace flake
{
namespace test
{
namespace information
{
typedef
std::function<information::pull_function_fn>
pull_function;
}
}
}

#endif

