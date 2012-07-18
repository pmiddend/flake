#ifndef FLAKE_SKYDOME_TEXTURE_PATH_HPP_INCLUDED
#define FLAKE_SKYDOME_TEXTURE_PATH_HPP_INCLUDED

#include <fcppt/strong_typedef.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/filesystem/path.hpp>
#include <fcppt/config/external_end.hpp>


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
