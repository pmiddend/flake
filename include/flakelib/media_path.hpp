#ifndef FLAKELIB_MEDIA_PATH_HPP_INCLUDED
#define FLAKELIB_MEDIA_PATH_HPP_INCLUDED

#include <flakelib/symbol.hpp>
#include <fcppt/filesystem/path.hpp>

namespace flakelib
{
FLAKELIB_SYMBOL
fcppt::filesystem::path const
media_path();
}

#endif
