#ifndef FLAKELIB_CL_PROGRAM_HPP_INCLUDED
#define FLAKELIB_CL_PROGRAM_HPP_INCLUDED

#include <sge/opencl/context/object_fwd.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <sge/opencl/program/object.hpp>
#include <flakelib/cl/compiler_flags.hpp>
#include <fcppt/noncopyable.hpp>

namespace flakelib
{
namespace cl
{
class program
{
FCPPT_NONCOPYABLE(
	program);
public:
	explicit
	program(
		sge::opencl::context::object &,
		flakelib::cl::compiler_flags const &);

	~program();
private:
	sge::opencl::program::object program_;
};
}
}

#endif

