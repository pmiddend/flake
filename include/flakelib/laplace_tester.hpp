#ifndef FLAKELIB_LAPLACE_TESTER_HPP_INCLUDED
#define FLAKELIB_LAPLACE_TESTER_HPP_INCLUDED

#include <flakelib/build_options.hpp>
#include <flakelib/viewport_pager.hpp>
#include <flakelib/laplace_solver/base_fwd.hpp>
#include <flakelib/monitor/parent.hpp>
#include <flakelib/monitor/planar_converter.hpp>
#include <flakelib/monitor/texture_fwd.hpp>
#include <flakelib/buffer_pool/object_fwd.hpp>
#include <flakelib/buffer_pool/planar_lock.hpp>
#include <flakelib/utility/object_fwd.hpp>
#include <rucksack/widget/enumeration.hpp>
#include <sge/font/system_fwd.hpp>
#include <sge/image2d/system_fwd.hpp>
#include <sge/image2d/file_ptr.hpp>
#include <sge/opencl/command_queue/object_fwd.hpp>
#include <sge/opencl/kernel/object.hpp>
#include <sge/opencl/memory_object/image/planar.hpp>
#include <sge/opencl/program/object.hpp>
#include <sge/parse/json/object_fwd.hpp>
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
		flakelib::buffer_pool::object &,
		flakelib::utility::object &,
		sge::renderer::device &,
		sge::viewport::manager &,
		sge::opencl::command_queue::object &,
		sge::font::system &,
		sge::image2d::system &,
		flakelib::build_options const &);

	void
	render(
		monitor::optional_projection const &);

	void
	update();

	~laplace_tester();
private:
	typedef
	boost::ptr_vector<monitor::texture>
	additional_data_monitors;

	laplace_solver::base &solver_;
	sge::opencl::command_queue::object &command_queue_;
	utility::object &utility_;
	sge::image2d::file_ptr boundary_image_file_;
	flakelib::buffer_pool::planar_lock<cl_float> boundary_;
	monitor::parent monitor_parent_;
	monitor::planar_converter planar_converter_;
	flakelib::viewport_pager master_widget_;
	rucksack::widget::enumeration enumeration_widget_;
	flakelib::buffer_pool::planar_lock<cl_float> initial_guess_image_;
	flakelib::buffer_pool::planar_lock<cl_float> rhs_;
	flakelib::buffer_pool::planar_lock<cl_float> destination_;
	additional_data_monitors additional_data_;
};
}

#endif
