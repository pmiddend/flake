#ifndef FLAKE_UTF8_FILE_TO_FCPPT_STRING_HPP_INCLUDED
#define FLAKE_UTF8_FILE_TO_FCPPT_STRING_HPP_INCLUDED

#include <fcppt/filesystem/path.hpp>
#include <fcppt/string.hpp>

namespace flake
{
fcppt::string const
utf8_file_to_fcppt_string(
	fcppt::filesystem::path const &);
}

#endif
