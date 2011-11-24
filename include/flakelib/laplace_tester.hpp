#ifndef FLAKELIB_LAPLACE_TESTER_HPP_INCLUDED
#define FLAKELIB_LAPLACE_TESTER_HPP_INCLUDED

#include <flakelib/viewport_pager.hpp>
#include <flakelib/laplace_solver/base_fwd.hpp>
#include <flakelib/planar_pool/object_fwd.hpp>
#include <flakelib/planar_pool/scoped_lock.hpp>
#include <flakelib/utility/object_fwd.hpp>
#include <flakelib/visualization/monitor/child_fwd.hpp>
#include <flakelib/visualization/monitor/parent.hpp>
#include <rucksack/widget/enumeration.hpp>
#include <sge/font/system_fwd.hpp>
#include <sge/image2d/multi_loader_fwd.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/kernel/object.hpp>
#include <sge/opencl/memory_object/image/planar.hpp>
#include <sge/opencl/program/object.hpp>
#include <sge/parse/json/object_fwd.hpp>
#include <sge/renderer/device_fwd.hpp>
#include <sge/viewport/manager_fwd.hpp>
#include <fcppt/noncopyable.hpp>
#include <fcppt/string.hpp>
#include <fcppt/unique_ptr.hpp>
#include <fcppt/config/external_begin.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <fcppt/config/external_end.hpp>


namespace flakelib
{
class laplace_tester
{
FCPPT_NONCOPYABLE(
	laplace_tester);
public:
	explicit
	laplace_tester(
		laplace_solver::base &,
		flakelib::planar_pool::object &,
		flakelib::utility::object &,
		sge::renderer::device &,
		sge::viewport::manager &,
		sge::opencl::command_queue::object &,
		sge::font::system &,
		sge::image2d::multi_loader &);

	void
	render(
		visualization::monitor::optional_projection const &);

	void
	update();

	~laplace_tester();
private:
	typedef
	boost::ptr_vector<visualization::monitor::child>
	additional_data_monitors;

	laplace_solver::base &solver_;
	sge::renderer::device &renderer_;
	sge::opencl::command_queue::object &command_queue_;
	flakelib::planar_pool::object &planar_cache_;
	utility::object &utility_;
	fcppt::unique_ptr<sge::opencl::memory_object::image::planar> boundary_;
	visualization::monitor::parent monitor_parent_;
	flakelib::viewport_pager master_widget_;
	rucksack::widget::enumeration enumeration_widget_;
	flakelib::planar_pool::scoped_lock initial_guess_image_;
	flakelib::planar_pool::scoped_lock rhs_;
	flakelib::planar_pool::scoped_lock destination_;
	additional_data_monitors additional_data_;
};
}

#endif
