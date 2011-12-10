#ifndef FLAKELIB_BUILD_OPTIONS_HPP_INCLUDED
#define FLAKELIB_BUILD_OPTIONS_HPP_INCLUDED

#include <sge/opencl/program/build_options.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::opencl::program::build_options,
	build_options);
}

#endif
