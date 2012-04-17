#ifndef FLAKELIB_CL_KERNEL_SHARED_PTR_HPP_INCLUDED
#define FLAKELIB_CL_KERNEL_SHARED_PTR_HPP_INCLUDED

#include <flakelib/cl/kernel_fwd.hpp>
#include <fcppt/shared_ptr.hpp>

namespace flakelib
{
namespace cl
{
typedef
fcppt::shared_ptr<cl::kernel>
kernel_shared_ptr;
}
}

#endif

