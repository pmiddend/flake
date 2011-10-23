#if 0
#ifndef FLAKELIB_PDE_TESTER_HPP_INCLUDED
#define FLAKELIB_PDE_TESTER_HPP_INCLUDED

#include <flakelib/visualization/monitor/parent_fwd.hpp>
#include <flakelib/visualization/monitor/texture_fwd.hpp>
#include <sge/font/system_fwd.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/context/object_fwd.hpp>
#include <sge/opencl/kernel/object.hpp>
#include <sge/opencl/memory_object/dim2.hpp>
#include <sge/opencl/memory_object/image/planar.hpp>
#include <sge/opencl/program/object.hpp>
#include <sge/parse/json/object_fwd.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <fcppt/scoped_ptr.hpp>

namespace flakelib
{
class pde_tester
{
FCPPT_NONCOPYABLE(
	pde_tester);
public:
	explicit
	pde_tester(
		sge::opencl::context::object &,
		sge::opencl::command_queue::object &,
		sge::renderer::device &,
		sge::font::system &,
		sge::parse::json::object const &,
		sge::opencl::memory_object::dim2 const &);

	void
	render();

	void
	update();

	~pde_tester();
private:
	typedef
	boost::ptr_vector<sge::opencl::memory_object::image::planar>
	image_pool;

	typedef
	std::set<sge::opencl::memory_object::image::planar*>
	image_set;

	sge::opencl::command_queue::object &command_queue_;
	sge::opencl::memory_object::image::planar p0_, p1_, h_;
	sge::opencl::program::object guess_generation_program_;
	sge::opencl::kernel::object generate_guess_kernel_;
	sge::opencl::program::object multigrid_program_;
	sge::opencl::program::object jacobi_program_;
	fcppt::scoped_ptr<visualization::monitor::parent> monitor_parent_;
	fcppt::scoped_ptr<visualization::monitor::texture> master_monitor_;
	image_pool images_;
	image_set unlocked_images_;

	void
	multigrid(
		sge::opencl::memory_object::image::planar &rhs,
		sge::opencl::memory_object::image::planar &destination);

	void
	null_image(
		sge::opencl::memory_object::image::planar &);

	void
	jacobi(
		sge::opencl::memory_object::image::planar &initial,
		sge::opencl::memory_object::image::planar &alternation,
		sge::opencl::memory_object::image::planar &rhs);

	void
	laplacian_residual_rhs_from_to(
		sge::opencl::memory_object::image::planar &rhs,
		sge::opencl::memory_object::image::planar &from,
		sge::opencl::memory_object::image::planar &to);

	void
	downsample_from_to(
		sge::opencl::memory_object::image::planar &from,
		sge::opencl::memory_object::image::planar &to);

	void
	upsample_from_to(
		sge::opencl::memory_object::image::planar &from,
		sge::opencl::memory_object::image::planar &to);

	void
	add_to(
		sge::opencl::memory_object::image::planar &from,
		sge::opencl::memory_object::image::planar &to,
		sge::opencl::memory_object::image::planar &result);

	void
	lock(
		sge::opencl::memory_object::image::planar &);

	void
	unlock(
		sge::opencl::memory_object::image::planar &);

	sge::opencl::memory_object::image::planar &
	unpool_image(
		std::size_t);
};
}

#endif
#endif
