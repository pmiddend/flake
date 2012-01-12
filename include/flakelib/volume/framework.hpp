#ifndef FLAKELIB_VOLUME_FRAMEWORK_HPP_INCLUDED
#define FLAKELIB_VOLUME_FRAMEWORK_HPP_INCLUDED

#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/memory_object/dim3.hpp>
#include <sge/parse/json/object_fwd.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/renderer/matrix4.hpp>
#include <sge/renderer/vector3.hpp>
#include <sge/image2d/system_fwd.hpp>
#include <flakelib/volume/laplace_solver/unique_base_ptr.hpp>
#include <flakelib/volume/visualization/shape_manager_fwd.hpp>
#include <flakelib/volume/visualization/arrows_fwd.hpp>
#include <flakelib/volume/visualization/arrows_manager_fwd.hpp>
#include <flakelib/volume/boundary/object_fwd.hpp>
#include <flakelib/volume/simulation/stam/object_fwd.hpp>
#include <flakelib/volume/conversion/object_fwd.hpp>
#include <flakelib/buffer_pool/object_fwd.hpp>
#include <flakelib/utility/object_fwd.hpp>
#include <flakelib/duration.hpp>
#include <flakelib/build_options.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/scoped_ptr.hpp>

namespace flakelib
{
namespace volume
{
class framework
{
FCPPT_NONCOPYABLE(
	framework);
public:
	explicit
	framework(
		sge::opencl::command_queue::object &,
		sge::renderer::device &,
		sge::image2d::system &,
		flakelib::build_options const &,
		sge::parse::json::object const &);

	void
	update(
		flakelib::duration const &);

	void
	render(
		sge::renderer::vector3 const &,
		sge::renderer::matrix4 const &);

	~framework();
private:
	fcppt::scoped_ptr<flakelib::utility::object> utility_;
	fcppt::scoped_ptr<buffer_pool::object> buffer_pool_;
	fcppt::scoped_ptr<boundary::object> boundary_;
	laplace_solver::unique_base_ptr laplace_solver_;
	fcppt::scoped_ptr<simulation::stam::object> simulation_;
	fcppt::scoped_ptr<visualization::shape_manager> shape_manager_;
	fcppt::scoped_ptr<conversion::object> conversion_;
	fcppt::scoped_ptr<visualization::arrows_manager> arrows_manager_;
	fcppt::scoped_ptr<visualization::arrows> arrows_;
};
}
}

#endif
