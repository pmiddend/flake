#include <flake/media_path.hpp>
#include <flake/build/flake_media_path.hpp>


boost::filesystem::path const
flake::media_path()
{
	return flake::build_flake_media_path();
}
