#include <flakelib/media_path.hpp>
#include <fcppt/filesystem/path.hpp>
#include <fcppt/config/external_begin.hpp>
#include <flake/build/media_path.hpp>
#include <fcppt/config/external_end.hpp>


fcppt::filesystem::path const
flakelib::media_path()
{
	return flake::build_media_path();
}
