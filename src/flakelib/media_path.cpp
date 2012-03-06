#include <flakelib/media_path.hpp>
#include <flakelib/build/media_path.hpp>
#include <fcppt/filesystem/path.hpp>


fcppt::filesystem::path const
flakelib::media_path()
{
	return flakelib::build_media_path();
}
