#ifndef FLAKELIB_MEDIA_PATH_FROM_STRING_HPP_INCLUDED
#define FLAKELIB_MEDIA_PATH_FROM_STRING_HPP_INCLUDED

#include <fcppt/filesystem/path.hpp>
#include <fcppt/string.hpp>

namespace flakelib
{
fcppt::filesystem::path const
media_path_from_string(
	fcppt::string const &);
}

#endif