#include <flakelib/media_path_from_string.hpp>
#include <flakelib/main_head.hpp>
#include <flakelib/cl/planar_image_view_to_cl_image.hpp>
#include <flakelib/utility/object.hpp>
#include <sge/image/capabilities_field.hpp>
#include <sge/image2d/file.hpp>
#include <sge/image2d/system.hpp>
#include <sge/image2d/view/const_object.hpp>
#include <sge/media/all_extensions.hpp>
#include <sge/opencl/single_device_system.hpp>
#include <sge/opencl/memory_object/image/planar.hpp>
#include <sge/systems/image2d.hpp>
#include <sge/systems/instance.hpp>
#include <sge/systems/list.hpp>
#include <fcppt/exception.hpp>
#include <fcppt/from_std_string.hpp>
#include <fcppt/text.hpp>
#include <fcppt/to_std_string.hpp>
#include <fcppt/container/bitfield/basic_impl.hpp>
#include <fcppt/filesystem/path_to_string.hpp>
#include <fcppt/io/cerr.hpp>
#include <fcppt/io/cout.hpp>
#include <fcppt/config/external_begin.hpp>
#include <cstddef>
#include <exception>
#include <iostream>
#include <ostream>
#include <fcppt/config/external_end.hpp>

FLAKELIB_MAIN_HEAD
try
{
#if 0
	if(argc != 2)
	{
		fcppt::io::cerr()
			<< FCPPT_TEXT("Usage: ")
			<< fcppt::from_std_string(argv[0])
			<< FCPPT_TEXT(" <image file name>\n");
		return EXIT_FAILURE;
	}

	sge::systems::instance sys(
		sge::systems::list()
			(sge::systems::image2d(
				sge::image::capabilities_field::null(),
				sge::media::all_extensions)));

	sge::opencl::single_device_system opencl_system(
		(sge::opencl::optional_renderer()),
		sge::opencl::context::optional_error_callback());

	flakelib::build_options global_build_options(
		"-I "+
		fcppt::to_std_string(
			fcppt::filesystem::path_to_string(
				flakelib::media_path_from_string(
					FCPPT_TEXT("kernels")))));

	fcppt::unique_ptr<sge::opencl::memory_object::image::planar> input_image(
		flakelib::cl::planar_image_view_to_cl_image(
			sys.image_system().load(
				fcppt::from_std_string(
					argv[1]))->view(),
			opencl_system.command_queue()));

	flakelib::utility::object utility(
		opencl_system.command_queue(),
		global_build_options);

	fcppt::io::cout()
		<< FCPPT_TEXT("The frobenius norm of the image (or the image's red channel if it's multichannel) is\n\n")
		<< utility.frobenius_norm(*input_image) << FCPPT_TEXT("\n");
#endif
}
catch(fcppt::exception const &e)
{
	fcppt::io::cerr() << FCPPT_TEXT("fcppt exception caught: ") << e.string() << FCPPT_TEXT("\n");
	return EXIT_FAILURE;
}
catch(std::exception const &e)
{
	std::cerr << "standard exception caught: " << e.what() << "\n";
	return EXIT_FAILURE;
}
catch(...)
{
	std::cerr << "unknown exception caught\n";
	return EXIT_FAILURE;
}
