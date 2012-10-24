#include <flakelib/media_path.hpp>
#include <flakelib/build/flakelib_media_path.hpp>


boost::filesystem::path const
flakelib::media_path()
{
	return flakelib::build_flakelib_media_path();
}
