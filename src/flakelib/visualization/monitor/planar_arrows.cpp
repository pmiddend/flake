#include <flakelib/visualization/monitor/planar_arrows.hpp>
#include <flakelib/visualization/monitor/parent.hpp>
#include <sge/opencl/memory_object/renderer_buffer_lock_mode.hpp>
#include <sge/shader/scoped.hpp>
#include <sge/shader/activate_everything.hpp>
#include <sge/shader/matrix.hpp>
#include <sge/shader/matrix_flags.hpp>
#include <sge/shader/object.hpp>
#include <sge/renderer/scoped_vertex_buffer.hpp>
#include <sge/renderer/first_index.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/vertex_count.hpp>
#include <sge/renderer/nonindexed_primitive_type.hpp>
#include <sge/renderer/viewport_size.hpp>
#include <sge/renderer/projection/orthogonal_wh.hpp>
#include <sge/renderer/projection/dim.hpp>
#include <sge/renderer/projection/near.hpp>
#include <sge/renderer/projection/far.hpp>
#include <sge/renderer/vertex_buffer.hpp>
#include <sge/renderer/size_type.hpp>
#include <sge/renderer/resource_flags.hpp>
#include <sge/renderer/vf/dynamic/part_index.hpp>
#include <fcppt/math/dim/structure_cast.hpp>
#include <fcppt/math/dim/arithmetic.hpp>
#include <fcppt/math/dim/output.hpp>
#include <fcppt/math/box/basic_impl.hpp>
#include <fcppt/io/cout.hpp>
#include <fcppt/text.hpp>

flakelib::visualization::monitor::planar_arrows::planar_arrows(
	monitor::parent &_parent,
	monitor::grid_dimensions const &_dimensions,
	monitor::arrow_scale const &_arrow_scale,
	monitor::grid_scale const &_grid_scale,
	monitor::position const &_position)
:
	parent_(
		_parent),
	dimensions_(
		_dimensions),
	arrow_scale_(
		_arrow_scale),
	grid_scale_(
		_grid_scale),
	position_(
		_position),
	vb_(
		parent_.renderer().create_vertex_buffer(
			parent_.vertex_declaration(),
			sge::renderer::vf::dynamic::part_index(
				0u),
			static_cast<sge::renderer::size_type>(
				dimensions_.content() * 2),
			sge::renderer::resource_flags::readable)),
	cl_vb_(
		parent_.context(),
		*vb_,
		sge::opencl::memory_object::renderer_buffer_lock_mode::write_only)
{
	fcppt::io::cout() 
		<< FCPPT_TEXT("Created planar arrows with ") 
		<< (dimensions_.content() * 2) 
		<< FCPPT_TEXT(" elements (dimension was ") 
		<< dimensions_ 
		<< FCPPT_TEXT(")\n");
}

void
flakelib::visualization::monitor::planar_arrows::from_buffer(
	flakelib::buffer_or_image const &_buffer_or_image)
{
	parent_.to_vb(
		_buffer_or_image,
		cl_vb_,
		grid_scale_,
		arrow_scale_);
}

void
flakelib::visualization::monitor::planar_arrows::render()
{
	// Activate the shader and the vertex declaration
	sge::shader::scoped scoped_shader(
		parent_.arrow_shader(),
		sge::shader::activate_everything());

	sge::renderer::scoped_vertex_buffer scoped_vb(
		parent_.renderer(),
		*vb_);

	parent_.arrow_shader().update_uniform(
		"projection",
		sge::shader::matrix(
			sge::renderer::projection::orthogonal_wh(
				fcppt::math::dim::structure_cast<sge::renderer::projection::dim>(
						sge::renderer::viewport_size(
							parent_.renderer())),
				sge::renderer::projection::near(0.0f),
				sge::renderer::projection::far(10.0f)),
			sge::shader::matrix_flags::projection));

	parent_.arrow_shader().update_uniform(
		"initial_position",
		position_.get());

	parent_.renderer().render_nonindexed(
		sge::renderer::first_vertex(
			0),
		sge::renderer::vertex_count(
			vb_->size()),
		sge::renderer::nonindexed_primitive_type::line);
}

flakelib::visualization::monitor::rect const
flakelib::visualization::monitor::planar_arrows::area() const
{
	return 
		monitor::rect(
			position_.get(),
			grid_scale_.get() * 
			fcppt::math::dim::structure_cast<monitor::rect::dim>(
				dimensions_));
}

flakelib::visualization::monitor::planar_arrows::~planar_arrows()
{
}
