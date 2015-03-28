#ifndef FLAKE_MEDIA_PATH_FROM_STRING_HPP_INCLUDED
#define FLAKE_MEDIA_PATH_FROM_STRING_HPP_INCLUDED

#include <flake/detail/symbol.hpp>
#include <fcppt/string.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/filesystem/path.hpp>
#include <fcppt/config/external_end.hpp>


namespace flake
{
FLAKE_DETAIL_SYMBOL
boost::filesystem::path
media_path_from_string(
	fcppt::string const &);
}

#endif

