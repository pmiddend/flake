#ifndef FLAKELIB_VOLUME_CONVERSION_RAW_VOXEL_FILE_DIMENSION_HPP_INCLUDED
#define FLAKELIB_VOLUME_CONVERSION_RAW_VOXEL_FILE_DIMENSION_HPP_INCLUDED

#include <fcppt/strong_typedef.hpp>
#include <sge/opencl/memory_object/size_type.hpp>

namespace flakelib
{
namespace volume
{
namespace conversion
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::opencl::memory_object::size_type,
	raw_voxel_file_dimension);
}
}
}

#endif

