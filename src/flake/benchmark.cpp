#include <flakelib/duration.hpp>
#include <flakelib/media_path_from_string.hpp>
#include <flakelib/utf8_file_to_fcppt_string.hpp>
#include <flakelib/simulation/base.hpp>
#include <flakelib/simulation/base_ptr.hpp>
#include <flakelib/simulation/create.hpp>
#include <sge/all_extensions.hpp>
#include <sge/image/capabilities_field.hpp>
#include <sge/image2d/file.hpp>
#include <sge/image2d/file_ptr.hpp>
#include <sge/image2d/multi_loader.hpp>
#include <sge/image2d/view/const_object.hpp>
#include <sge/log/global_context.hpp>
#include <sge/opencl/single_device_system.hpp>
#include <sge/parse/json/array.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <sge/parse/json/object.hpp>
#include <sge/parse/json/parse_string_exn.hpp>
#include <sge/parse/json/path.hpp>
#include <sge/parse/json/config/create_command_line_parameters.hpp>
#include <sge/parse/json/config/merge_command_line_parameters.hpp>
#include <sge/systems/image_loader.hpp>
#include <sge/systems/instance.hpp>
#include <sge/systems/list.hpp>
#include <fcppt/exception.hpp>
#include <fcppt/text.hpp>
#include <fcppt/chrono/duration_impl.hpp>
#include <fcppt/chrono/seconds.hpp>
#include <fcppt/container/bitfield/basic_impl.hpp>
#include <fcppt/io/cerr.hpp>
#include <fcppt/log/activate_levels.hpp>
#include <fcppt/log/context.hpp>
#include <fcppt/log/level.hpp>
#include <fcppt/log/location.hpp>
#include <fcppt/math/dim/basic_impl.hpp>
#include <fcppt/signal/scoped_connection.hpp>
#include <fcppt/config/external_begin.hpp>
#include <cstddef>
#include <exception>
#include <iostream>
#include <ostream>
#include <fcppt/config/external_end.hpp>


int
main(
	int argc,
	char *argv[])
try
{
	sge::log::global_context().apply(
		fcppt::log::location(
			FCPPT_TEXT("opencl")),
		std::tr1::bind(
			&fcppt::log::activate_levels,
			std::tr1::placeholders::_1,
			fcppt::log::level::verbose));

	sge::parse::json::object const config_file =
		sge::parse::json::config::merge_command_line_parameters(
			sge::parse::json::parse_string_exn(
				flakelib::utf8_file_to_fcppt_string(
					flakelib::media_path_from_string(
						FCPPT_TEXT("config.json")))),
			sge::parse::json::config::create_command_line_parameters(
				argc,
				argv));

	sge::systems::instance sys(
		sge::systems::list()
			(sge::systems::image_loader(
				sge::image::capabilities_field::null(),
				sge::all_extensions)));

	sge::opencl::single_device_system opencl_system(
		(sge::opencl::optional_renderer()),
		(sge::opencl::context::optional_error_callback()));

	sge::image2d::file_ptr boundary_image =
		sys.image_loader().load(
			flakelib::media_path_from_string(
				FCPPT_TEXT("images/boundary.png")));

	flakelib::simulation::base_ptr simulation(
		flakelib::simulation::create(
			opencl_system.context(),
			opencl_system.command_queue(),
			flakelib::boundary_view(
				boundary_image->view()),
			config_file));

	for(
		unsigned
			i =
				0,
			its =
				sge::parse::json::find_and_convert_member<unsigned>(
					config_file,
					sge::parse::json::path(
						FCPPT_TEXT("benchmark-iterations")));
		i < its;
		++i)
	{
		simulation->update(
			flakelib::duration(
				0.01f));
	}
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

