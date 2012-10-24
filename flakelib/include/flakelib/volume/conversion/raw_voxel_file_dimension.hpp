#ifndef FLAKELIB_VOLUME_CONVERSION_RAW_VOXEL_FILE_DIMENSION_HPP_INCLUDED
#define FLAKELIB_VOLUME_CONVERSION_RAW_VOXEL_FILE_DIMENSION_HPP_INCLUDED

#include <sge/opencl/size_type.hpp>
#include <fcppt/strong_typedef.hpp>


namespace flakelib
{
namespace volume
{
namespace conversion
{
FCPPT_MAKE_STRONG_TYPEDEF(
	sge::opencl::size_type,
	raw_voxel_file_dimension);
}
}
}

#endif

