#ifndef FLAKE_PLANAR_CONVERSION_X_COORDINATE_FILE_HPP_INCLUDED
#define FLAKE_PLANAR_CONVERSION_X_COORDINATE_FILE_HPP_INCLUDED

#include <boost/filesystem/path.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flake
{
namespace planar
{
namespace conversion
{
FCPPT_MAKE_STRONG_TYPEDEF(
	boost::filesystem::path,
	x_coordinate_file);
}
}
}

#endif

