#include <flake/media_path.hpp>
#include <flake/build/media_path.hpp>


boost::filesystem::path const
flake::media_path()
{
	return flake::build_media_path();
}
