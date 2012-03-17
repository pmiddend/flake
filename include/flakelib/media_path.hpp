#ifndef FLAKELIB_MEDIA_PATH_HPP_INCLUDED
#define FLAKELIB_MEDIA_PATH_HPP_INCLUDED

#include <flakelib/symbol.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/filesystem/path.hpp>
#include <fcppt/config/external_end.hpp>


namespace flakelib
{
FLAKELIB_SYMBOL
boost::filesystem::path const
media_path();
}

#endif
