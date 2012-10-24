#ifndef FLAKE_TESTS_SCAN_HPP_INCLUDED
#define FLAKE_TESTS_SCAN_HPP_INCLUDED

#include <flakelib/buffer_pool/object.hpp>
#include <flakelib/cl/program_context.hpp>
#include <flakelib/scan/object.hpp>
#include <flakelib/volume/grid_size.hpp>
#include <flakelib/volume/unique_float4_buffer_lock.hpp>
#include <flakelib/volume/unique_float_buffer_lock.hpp>
#include <sge/charconv/system_scoped_ptr.hpp>
#include <sge/opencl/single_device_system/object.hpp>
#include <sge/parse/json/object.hpp>
#include <awl/main/exit_code.hpp>
#include <awl/main/function_context_fwd.hpp>


namespace flake
{
namespace tests
{
class scan
{
FCPPT_NONCOPYABLE(
	scan);
public:
	explicit
	scan(
		awl::main::function_context const &);

	awl::main::exit_code const
	run();

	~scan();
private:
	sge::opencl::single_device_system::object opencl_system_;
	sge::charconv::system_scoped_ptr charconv_system_;
	sge::parse::json::object json_configuration_;
	flakelib::cl::program_context program_context_;
	flakelib::buffer_pool::object buffer_pool_;
	flakelib::scan::object scan_;

	void
	run_once();
};
}
}

#endif

