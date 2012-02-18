#ifndef FLAKELIB_PROGRAM_UNIQUE_PTR_HPP_INCLUDED
#define FLAKELIB_PROGRAM_UNIQUE_PTR_HPP_INCLUDED

#include <sge/opencl/program/object_fwd.hpp>
#include <fcppt/unique_ptr.hpp>

namespace flakelib
{
namespace program
{
typedef
fcppt::unique_ptr<sge::opencl::program::object>
unique_ptr;
}
}

#endif

