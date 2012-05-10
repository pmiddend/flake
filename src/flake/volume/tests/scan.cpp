#include <flake/catch_statements.hpp>
#include <flake/media_path_from_string.hpp>
#include <flake/test/information/string_conversion_adapter.hpp>
#include <flake/volume/tests/scan.hpp>
#include <flakelib/media_path_from_string.hpp>
#include <flakelib/utf8_file_to_fcppt_string.hpp>
#include <flakelib/buffer/linear_view_impl.hpp>
#include <flakelib/buffer_pool/volume_lock_impl.hpp>
#include <flakelib/cl/cflags.hpp>
#include <flakelib/volume/retrieve_filled_float_buffer.hpp>
#include <flakelib/volume/retrieve_zero_float4_buffer.hpp>
#include <sge/camera/coordinate_system/identity.hpp>
#include <sge/camera/first_person/parameters.hpp>
#include <sge/charconv/create_system.hpp>
#include <sge/charconv/system.hpp>
#include <sge/image/colors.hpp>
#include <sge/opencl/command_queue/scoped_buffer_mapping.hpp>
#include <sge/opencl/single_device_system/object.hpp>
#include <sge/opencl/single_device_system/parameters.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <sge/parse/json/parse_string_exn.hpp>
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
#include <awl/main/exit_success.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/to_std_string.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/assign/make_container.hpp>
#include <fcppt/container/bitfield/object_impl.hpp>
#include <fcppt/filesystem/path_to_string.hpp>
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
	flake::volume::tests::scan s(
		_function_context);

	return
		s.run();
}
FLAKE_CATCH_STATEMENTS

flake::volume::tests::scan::scan(
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
	buffer_pool_(
		opencl_system_.context()),
	scan_(
		program_context_,
		buffer_pool_)
{
}

flake::volume::tests::scan::~scan()
{
}

awl::main::exit_code const
flake::volume::tests::scan::run()
{
	flakelib::scan::object::unique_linear_uint_lock source_buffer(
		fcppt::make_unique_ptr<flakelib::scan::object::linear_uint_lock>(
			fcppt::ref(
				buffer_pool_),
			sge::opencl::memory_object::dim1(
				64u * 128u * 128u)));

	{
		sge::opencl::command_queue::scoped_buffer_mapping buffer_mapping(
			opencl_system_.command_queue(),
			source_buffer->value().buffer(),
			CL_MAP_WRITE,
			sge::opencl::memory_object::byte_offset(
				0u),
			source_buffer->value().buffer().byte_size());

		cl_uint *ptr =
			static_cast<cl_uint *>(
				buffer_mapping.ptr());

		for(
			cl_uint i =
				0;
			i < static_cast<cl_uint>(source_buffer->value().size().w());
			++i)
			ptr[static_cast<std::size_t>(i)] =
				1u;
	}

	flakelib::scan::object::unique_linear_uint_lock destination_buffer(
		scan_.update(
			source_buffer->value(),
			flakelib::scan::batch_size(
				1u)));

	{
		sge::opencl::command_queue::scoped_buffer_mapping buffer_mapping(
			opencl_system_.command_queue(),
			destination_buffer->value().buffer(),
			CL_MAP_READ,
			sge::opencl::memory_object::byte_offset(
				0u),
			destination_buffer->value().buffer().byte_size());

		cl_uint *ptr =
			static_cast<cl_uint *>(
				buffer_mapping.ptr());

		for(
			cl_uint i =
				0;
			i < static_cast<cl_uint>(destination_buffer->value().size().w());
			++i)
			std::cout << i << ": " << ptr[i] << "\n";
	}


	return
		awl::main::exit_success();
}