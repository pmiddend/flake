#ifndef FLAKELIB_UTF8_FILE_TO_FCPPT_STRING_HPP_INCLUDED
#define FLAKELIB_UTF8_FILE_TO_FCPPT_STRING_HPP_INCLUDED

#include <flakelib/symbol.hpp>
#include <sge/charconv/system_fwd.hpp>
#include <fcppt/string.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/filesystem/path.hpp>
#include <fcppt/config/external_end.hpp>


namespace flakelib
{
FLAKELIB_SYMBOL
fcppt::string const
utf8_file_to_fcppt_string(
	sge::charconv::system &,
	boost::filesystem::path const &);
}

#endif
