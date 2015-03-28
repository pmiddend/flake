#ifndef FLAKE_MEDIA_PATH_HPP_INCLUDED
#define FLAKE_MEDIA_PATH_HPP_INCLUDED

#include <flake/detail/symbol.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/filesystem/path.hpp>
#include <fcppt/config/external_end.hpp>


namespace flake
{
FLAKE_DETAIL_SYMBOL
boost::filesystem::path
media_path();
}

#endif

