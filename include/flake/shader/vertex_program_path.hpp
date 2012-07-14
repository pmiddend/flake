#ifndef FLAKE_SHADER_VERTEX_PROGRAM_PATH_HPP_INCLUDED
#define FLAKE_SHADER_VERTEX_PROGRAM_PATH_HPP_INCLUDED

#include <fcppt/strong_typedef.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/filesystem/path.hpp>
#include <fcppt/config/external_end.hpp>

namespace flake
{
namespace shader
{
FCPPT_MAKE_STRONG_TYPEDEF(
	boost::filesystem::path,
	vertex_program_path);
}
}

#endif
