#ifndef FLAKE_TEST_INFORMATION_PULL_FUNCTION_HPP_INCLUDED
#define FLAKE_TEST_INFORMATION_PULL_FUNCTION_HPP_INCLUDED

#include <flake/test/information/pull_function_fn.hpp>
#include <fcppt/function/object.hpp>

namespace flake
{
namespace test
{
namespace information
{
typedef
fcppt::function::object<information::pull_function_fn>
pull_function;
}
}
}

#endif

