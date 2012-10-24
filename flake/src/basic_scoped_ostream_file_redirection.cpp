#include <flake/basic_scoped_ostream_file_redirection_impl.hpp>

template class flake::basic_scoped_ostream_file_redirection<char, std::char_traits<char> >;
template class flake::basic_scoped_ostream_file_redirection<wchar_t, std::char_traits<wchar_t> >;
