#include <flake/build/media_path.hpp>
#include <flakelib/media_path.hpp>
#include <fcppt/filesystem/path.hpp>


fcppt::filesystem::path const
flakelib::media_path()
{
	return flake::build_media_path();
}
