#ifndef FLAKELIB_PROGRAM_BUILD_OPTIONS_HPP_INCLUDED
#define FLAKELIB_PROGRAM_BUILD_OPTIONS_HPP_INCLUDED

#include <sge/opencl/program/build_options.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace program
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::opencl::program::build_options,
	build_options);
}
}

#endif
