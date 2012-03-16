#ifndef FLAKELIB_CL_UNIQUE_KERNEL_PTR_HPP_INCLUDED
#define FLAKELIB_CL_UNIQUE_KERNEL_PTR_HPP_INCLUDED

#include <flakelib/cl/kernel_fwd.hpp>
#include <fcppt/unique_ptr.hpp>

namespace flakelib
{
namespace cl
{
typedef
fcppt::unique_ptr<kernel>
unique_kernel_ptr;
}
}

#endif

