#ifndef FLAKELIB_PROGRAM_CREATE_FROM_CONTEXT_HPP_INCLUDED
#define FLAKELIB_PROGRAM_CREATE_FROM_CONTEXT_HPP_INCLUDED

#include <flakelib/program/context_fwd.hpp>
#include <flakelib/program/unique_ptr.hpp>
#include <fcppt/filesystem/path.hpp>

namespace flakelib
{
namespace program
{
program::unique_ptr
create_from_context(
	program::context const &,
	fcppt::filesystem::path const &);
}
}

#endif

