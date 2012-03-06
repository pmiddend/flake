#include <flakelib/media_path_from_string.hpp>
#include <flakelib/cl/cflags.hpp>
#include <fcppt/text.hpp>
#include <fcppt/to_std_string.hpp>
#include <fcppt/filesystem/path_to_string.hpp>


std::string const
flakelib::cl::cflags()
{
	return
		"-I"+
		fcppt::to_std_string(
			fcppt::filesystem::path_to_string(
				flakelib::media_path_from_string(
					FCPPT_TEXT("kernels"))));
}
