#ifndef FLAKE_SKYDOME_TEXTURE_PATH_HPP_INCLUDED
#define FLAKE_SKYDOME_TEXTURE_PATH_HPP_INCLUDED

#include <boost/filesystem/path.hpp>
#include <fcppt/strong_typedef.hpp>

namespace flake
{
namespace skydome
{
FCPPT_MAKE_STRONG_TYPEDEF(
	boost::filesystem::path,
	texture_path);
}
}

#endif
