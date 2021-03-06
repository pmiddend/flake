#ifndef FLAKELIB_SCAN_BATCH_SIZE_HPP_INCLUDED
#define FLAKELIB_SCAN_BATCH_SIZE_HPP_INCLUDED

#include <sge/opencl/size_type.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flakelib
{
namespace scan
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::opencl::size_type,
	batch_size);
}
}

#endif

