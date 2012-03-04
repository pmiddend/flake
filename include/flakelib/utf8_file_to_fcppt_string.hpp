#ifndef FLAKELIB_UTF8_FILE_TO_FCPPT_STRING_HPP_INCLUDED
#define FLAKELIB_UTF8_FILE_TO_FCPPT_STRING_HPP_INCLUDED

#include <flakelib/symbol.hpp>
#include <fcppt/string.hpp>
#include <fcppt/filesystem/path.hpp>


namespace flakelib
{
FLAKELIB_SYMBOL
fcppt::string const
utf8_file_to_fcppt_string(
	fcppt::filesystem::path const &);
}

#endif
