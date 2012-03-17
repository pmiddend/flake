#include <flake/media_path.hpp>
#include <flake/build/media_path.hpp>
#include <fcppt/filesystem/path.hpp>


fcppt::filesystem::path const
flake::media_path()
{
	return flake::build_media_path();
}
