#ifndef FLAKE_SCOPED_WOSTREAM_FILE_REDIRECTION_HPP_INCLUDED
#define FLAKE_SCOPED_WOSTREAM_FILE_REDIRECTION_HPP_INCLUDED

#include <flake/basic_scoped_ostream_file_redirection_decl.hpp>

namespace flake
{
typedef
flake::basic_scoped_ostream_file_redirection<wchar_t>
scoped_wostream_file_redirection;
}

#endif

