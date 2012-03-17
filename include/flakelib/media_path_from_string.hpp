#ifndef FLAKELIB_MEDIA_PATH_FROM_STRING_HPP_INCLUDED
#define FLAKELIB_MEDIA_PATH_FROM_STRING_HPP_INCLUDED

#include <flakelib/symbol.hpp>
#include <fcppt/string.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/filesystem/path.hpp>
#include <fcppt/config/external_end.hpp>


namespace flakelib
{
FLAKELIB_SYMBOL
boost::filesystem::path const
media_path_from_string(
	fcppt::string const &);
}

#endif
