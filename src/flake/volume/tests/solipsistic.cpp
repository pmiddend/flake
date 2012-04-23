#include <awl/main/exit_success.hpp>
#include <fcppt/assert/pre.hpp>
#include <sge/charconv/system.hpp>
#include <flake/media_path_from_string.hpp>
#include <flakelib/utf8_file_to_fcppt_string.hpp>
#include <fcppt/filesystem/path_to_string.hpp>
#include <fcppt/to_std_string.hpp>
#include <flakelib/cl/cflags.hpp>
#include <flakelib/media_path_from_string.hpp>
#include <sge/parse/json/parse_string_exn.hpp>
#include <sge/charconv/create_system.hpp>
#include <sge/opencl/single_device_system/parameters.hpp>
#include <flake/catch_statements.hpp>
#include <flake/test/information/string_conversion_adapter.hpp>
#include <flake/volume/tests/solipsistic.hpp>
#include <flakelib/buffer/linear_view_impl.hpp>
#include <flakelib/buffer_pool/volume_lock_impl.hpp>
#include <flakelib/volume/retrieve_zero_float4_buffer.hpp>
#include <flakelib/volume/retrieve_zero_float_buffer.hpp>
#include <sge/camera/coordinate_system/identity.hpp>
#include <sge/camera/first_person/parameters.hpp>
#include <sge/image/colors.hpp>
#include <sge/opencl/single_device_system/object.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <sge/parse/json/string_to_path.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/state/color.hpp>
#include <sge/renderer/state/draw_mode.hpp>
#include <sge/renderer/state/float.hpp>
#include <sge/renderer/state/list.hpp>
#include <sge/renderer/state/scoped.hpp>
#include <sge/timer/elapsed_and_reset.hpp>
#include <sge/timer/parameters.hpp>
#include <sge/timer/reset_when_expired.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/assign/make_container.hpp>
#include <fcppt/container/bitfield/object_impl.hpp>
#include <fcppt/math/deg_to_rad.hpp>
#include <fcppt/math/dim/object_impl.hpp>
#include <fcppt/math/vector/object_impl.hpp>
#include <fcppt/tr1/functional.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/chrono.hpp>
#include <fcppt/config/external_end.hpp>


awl::main::exit_code const
custom_main(
	awl::main::function_context const &);

awl::main::exit_code const
custom_main(
	awl::main::function_context const &_function_context)
try
{
	flake::volume::tests::solipsistic s(
		_function_context);

	return
		s.run();
}
FLAKE_CATCH_STATEMENTS

flake::volume::tests::solipsistic::solipsistic(
	awl::main::function_context const &)
:
	opencl_system_(
		sge::opencl::single_device_system::parameters()
			.prefer_gpu(
				true)),
	charconv_system_(
		sge::charconv::create_system()),
	json_configuration_(
		sge::parse::json::parse_string_exn(
			flakelib::utf8_file_to_fcppt_string(
				*charconv_system_,
				flake::media_path_from_string(
					FCPPT_TEXT("config.json"))))),
	simulation_size_(
		sge::parse::json::find_and_convert_member<sge::opencl::memory_object::dim3>(
			json_configuration_,
			sge::parse::json::string_to_path(
				FCPPT_TEXT("tests/solipsistic/simulation-size")))),
	program_context_(
		opencl_system_.command_queue(),
		flakelib::cl::compiler_flags(
			flakelib::cl::cflags()+
			" "+
			fcppt::to_std_string(
				sge::parse::json::find_and_convert_member<fcppt::string>(
					json_configuration_,
					sge::parse::json::string_to_path(
						FCPPT_TEXT("tests/opencl-compiler-flags")))+
				FCPPT_TEXT(" -I")+
				fcppt::filesystem::path_to_string(
					flake::media_path_from_string(
						FCPPT_TEXT("kernels")))))),
	fill_buffer_(
		program_context_),
	buffer_pool_(
		opencl_system_.context()),
	wind_source_(
		program_context_,
		flakelib::volume::simulation::stam::wind_strength(
			sge::parse::json::find_and_convert_member<cl_float>(
				json_configuration_,
				sge::parse::json::string_to_path(
					FCPPT_TEXT("tests/solipsistic/wind-strength-start"))))),
	outflow_boundaries_(
		program_context_),
	semilagrangian_advection_(
		program_context_,
		buffer_pool_),
	divergence_(
		program_context_,
		buffer_pool_),
	jacobi_(
		program_context_,
		buffer_pool_,
		flakelib::volume::simulation::stam::iterations(
			sge::parse::json::find_and_convert_member<flakelib::volume::simulation::stam::iterations::value_type>(
				json_configuration_,
				sge::parse::json::string_to_path(
					FCPPT_TEXT("tests/solipsistic/jacobi-iterations"))))),
	subtract_pressure_gradient_(
		program_context_),
	boundary_buffer_(
		flakelib::volume::retrieve_zero_float_buffer(
			buffer_pool_,
			fill_buffer_,
			simulation_size_.get())),
	velocity_buffer_(
		flakelib::volume::retrieve_zero_float4_buffer(
			buffer_pool_,
			fill_buffer_,
			simulation_size_.get()))
{
}

flake::volume::tests::solipsistic::~solipsistic()
{
}

awl::main::exit_code const
flake::volume::tests::solipsistic::run()
{
	unsigned const iteration_count =
		sge::parse::json::find_and_convert_member<unsigned>(
			json_configuration_,
			sge::parse::json::string_to_path(
				FCPPT_TEXT("tests/solipsistic/iterations")));

	FCPPT_ASSERT_PRE(
		iteration_count > 0);

	std::cout << "Doing " << iteration_count << "+1 iterations...\n";

	for(
		unsigned i = 0;
		i <= iteration_count;
		++i)
	{
		std::cout << '\r';
		std::cout << "|";
		std::cout << std::string(i,'-');
		std::cout << std::string(iteration_count-i,' ');
		std::cout << "|";

		this->run_once();
	}

	std::cout << "\nDone\n";

	return
		awl::main::exit_success();
}

void
flake::volume::tests::solipsistic::run_once()
{
	flakelib::duration const delta(0.1f);

	// Advection
	velocity_buffer_ =
		semilagrangian_advection_.update_float4(
			flakelib::volume::boundary_buffer_view(
				boundary_buffer_->value()),
			flakelib::volume::simulation::stam::velocity(
				velocity_buffer_->value()),
			velocity_buffer_->value(),
			delta);

	// Wind source
	wind_source_.update(
		velocity_buffer_->value());

	// Outflow boundaries
	outflow_boundaries_.update(
		velocity_buffer_->value());

	// Projection
	flakelib::volume::unique_float_buffer_lock divergence =
		divergence_.update(
			flakelib::volume::boundary_buffer_view(
				boundary_buffer_->value()),
			velocity_buffer_->value());

	flakelib::volume::unique_float_buffer_lock initial_guess_buffer_lock(
		fcppt::make_unique_ptr<flakelib::volume::float_buffer_lock>(
			fcppt::ref(
				buffer_pool_),
			velocity_buffer_->value().size()));

	fill_buffer_.apply(
		flakelib::buffer::linear_view<cl_float>(
			initial_guess_buffer_lock->value().buffer()),
		static_cast<cl_float>(
			0));

	flakelib::volume::unique_float_buffer_lock pressure =
		jacobi_.update(
			flakelib::volume::simulation::stam::initial_guess_buffer_view(
				initial_guess_buffer_lock->value()),
			flakelib::volume::boundary_buffer_view(
				boundary_buffer_->value()),
			flakelib::volume::simulation::stam::rhs_buffer_view(
				divergence->value()));

	subtract_pressure_gradient_.update(
		flakelib::volume::velocity_buffer_view(
			velocity_buffer_->value()),
		flakelib::volume::boundary_buffer_view(
			boundary_buffer_->value()),
		flakelib::volume::simulation::stam::pressure_buffer_view(
			pressure->value()));
}
