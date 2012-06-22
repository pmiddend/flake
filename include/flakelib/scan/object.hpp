#ifndef FLAKELIB_SCAN_OBJECT_HPP_INCLUDED
#define FLAKELIB_SCAN_OBJECT_HPP_INCLUDED

#include <flakelib/symbol.hpp>
#include <flakelib/buffer_pool/linear_lock_impl.hpp>
#include <flakelib/buffer_pool/object_fwd.hpp>
#include <flakelib/cl/kernel_scoped_ptr.hpp>
#include <flakelib/cl/program.hpp>
#include <flakelib/cl/program_context_fwd.hpp>
#include <flakelib/scan/batch_size.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/unique_ptr.hpp>


namespace flakelib
{
namespace scan
{
class object
{
FCPPT_NONCOPYABLE(
	object);
public:
	typedef
	flakelib::buffer::linear_view<cl_uint>
	linear_uint_view;

	typedef
	flakelib::buffer_pool::linear_lock<cl_uint>
	linear_uint_lock;

	typedef
	fcppt::unique_ptr<linear_uint_lock>
	unique_linear_uint_lock;

	FLAKELIB_SYMBOL
	object(
		flakelib::cl::program_context const &,
		flakelib::buffer_pool::object &);

	FLAKELIB_SYMBOL
	unique_linear_uint_lock
	update(
		linear_uint_view const &,
		flakelib::scan::batch_size const &);

	FLAKELIB_SYMBOL
	~object();
private:
	flakelib::buffer_pool::object &buffer_pool_;
	flakelib::cl::program program_;
	flakelib::cl::kernel_scoped_ptr exclusive_local1_kernel_;
	flakelib::cl::kernel_scoped_ptr exclusive_local2_kernel_;
	flakelib::cl::kernel_scoped_ptr uniform_update_kernel_;
	unique_linear_uint_lock buffer_;
	unique_linear_uint_lock debug_buffer_;

	FCPPT_MAKE_STRONG_TYPEDEF(
		linear_uint_view,
		source);

	FCPPT_MAKE_STRONG_TYPEDEF(
		linear_uint_view,
		destination);

	FCPPT_MAKE_STRONG_TYPEDEF(
		cl_uint,
		workgroup_count);

	FCPPT_MAKE_STRONG_TYPEDEF(
		cl_uint,
		elements_per_workgroup);

	FCPPT_MAKE_STRONG_TYPEDEF(
		cl_uint,
		block_count);

	void
	exclusive_local1(
		source const &,
		destination const &,
		workgroup_count const &,
		elements_per_workgroup const &);

	void
	exclusive_local2(
		source const &,
		destination const &,
		flakelib::scan::batch_size const &,
		block_count const &);

	void
	uniform_update(
		destination const &,
		block_count const &);

#ifdef FLAKELIB_SCAN_DEBUG
	void
	check_debug_buffer();
#endif
};
}
}

#endif

