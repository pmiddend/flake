#ifndef FLAKE_PLANAR_CONVERSION_X_COORDINATE_FILE_HPP_INCLUDED
#define FLAKE_PLANAR_CONVERSION_X_COORDINATE_FILE_HPP_INCLUDED

#include <fcppt/strong_typedef.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/filesystem/path.hpp>
#include <fcppt/config/external_end.hpp>


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

