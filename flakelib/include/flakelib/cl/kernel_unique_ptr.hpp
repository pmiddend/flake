#ifndef FLAKELIB_CL_KERNEL_UNIQUE_PTR_HPP_INCLUDED
#define FLAKELIB_CL_KERNEL_UNIQUE_PTR_HPP_INCLUDED

#include <flakelib/cl/kernel_fwd.hpp>
#include <fcppt/config/external_begin.hpp>
#include <memory>
#include <fcppt/config/external_end.hpp>

namespace flakelib
{
namespace cl
{
typedef
std::unique_ptr<cl::kernel>
kernel_unique_ptr;
}
}

#endif

