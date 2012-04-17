#ifndef FLAKELIB_CL_KERNEL_SCOPED_PTR_HPP_INCLUDED
#define FLAKELIB_CL_KERNEL_SCOPED_PTR_HPP_INCLUDED

#include <flakelib/cl/kernel_fwd.hpp>
#include <fcppt/scoped_ptr.hpp>

namespace flakelib
{
namespace cl
{
typedef
fcppt::scoped_ptr<cl::kernel>
kernel_scoped_ptr;
}
}

#endif

