#ifndef FLAKELIB_MARCHING_CUBES_SCAN_HPP_INCLUDED
#define FLAKELIB_MARCHING_CUBES_SCAN_HPP_INCLUDED

#include <flakelib/cl/program_context_fwd.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/kernel/object.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <sge/opencl/program/object.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/config/external_begin.hpp>
#include <cstddef>
#include <fcppt/config/external_end.hpp>


namespace flakelib
{
namespace marching_cubes
{
class scan
{
FCPPT_NONCOPYABLE(
	scan);
public:
	explicit
	scan(
		flakelib::cl::program_context const &);

	std::size_t
	scanExclusiveLarge(
		cl_mem d_Dst,
		cl_mem d_Src,
		cl_uint batchSize,
		cl_uint arrayLength);

	~scan();
private:
	sge::opencl::command_queue::object &command_queue_;
	sge::opencl::program::object program_;
	sge::opencl::kernel::object ckScanExclusiveLocal1;
	sge::opencl::kernel::object ckScanExclusiveLocal2;
	sge::opencl::kernel::object ckUniformUpdate;
	sge::opencl::memory_object::buffer buffer_;

	std::size_t
	scanExclusiveLocal1(
	    cl_mem d_Dst,
	    cl_mem d_Src,
	    cl_uint n,
	    cl_uint size);

	std::size_t
	scanExclusiveShort(
	    cl_mem d_Dst,
	    cl_mem d_Src,
	    cl_uint batchSize,
	    cl_uint arrayLength);

	void
	scanExclusiveLocal2(
	    cl_mem d_Buffer,
	    cl_mem d_Dst,
	    cl_mem d_Src,
	    cl_uint n,
	    cl_uint size);

	std::size_t
	uniformUpdate(
	    cl_mem d_Dst,
	    cl_mem d_Buffer,
	    cl_uint n);
};
}
}

#endif

