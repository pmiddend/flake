#include <sge/renderer/vertex_declaration.hpp>
#include <sge/renderer/state/scoped.hpp>
#include <flake/media_path_from_string.hpp>
#include <fcppt/math/matrix/arithmetic.hpp>
#include <sge/renderer/context/object.hpp>
#include <sge/camera/matrix_conversion/rotation.hpp>
#include <sge/camera/coordinate_system/object.hpp>
#include <sge/camera/base.hpp>
#include <sge/renderer/state/list.hpp>
#include <sge/renderer/state/depth_func.hpp>
#include <sge/renderer/scoped_vertex_declaration.hpp>
#include <sge/renderer/scoped_vertex_buffer.hpp>
#include <flake/shader/scoped_pair.hpp>
#include <flake/skydome/object.hpp>
#include <flake/skydome/index_format.hpp>
#include <sge/renderer/scoped_vertex_lock.hpp>
#include <sge/renderer/vf/view.hpp>
#include <sge/renderer/vf/vertex.hpp>
#include <sge/renderer/vf/iterator.hpp>
#include <fcppt/math/pi.hpp>
#include <fcppt/math/twopi.hpp>
#include <sge/renderer/scalar.hpp>
#include <sge/renderer/index_buffer.hpp>
#include <sge/renderer/vertex_buffer.hpp>
#include <cmath>
#include <fcppt/math/vector/static.hpp>
#include <fcppt/math/vector/object_impl.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/vf/dynamic/make_format.hpp>
#include <flake/skydome/vf/format.hpp>
#include <flake/skydome/vf/format_part.hpp>
#include <flake/skydome/vf/position.hpp>
#include <sge/renderer/vf/dynamic/make_part_index.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/index/dynamic/make_format.hpp>
#include <sge/renderer/vector3.hpp>
#include <sge/renderer/scoped_index_lock.hpp>
#include <sge/renderer/index/view.hpp>
#include <sge/renderer/index/iterator.hpp>
#include <fcppt/math/vector/arithmetic.hpp>

namespace
{
sge::renderer::vector3 const
sphere_point(
	sge::renderer::scalar const radius,
	sge::renderer::scalar const lat,
	sge::renderer::scalar const lon)
{
	return
		radius *
		sge::renderer::vector3(
			std::cos(lon) * std::sin(lat),
			std::cos(lat),
			std::sin(lon) * std::sin(lat));
}
}

flake::skydome::object::object(
	sge::renderer::device &_renderer,
	flake::shader::context &_shader_context,
	sge::image2d::system &_image_system,
	sge::camera::base &_camera,
	flake::skydome::longitude const &_longitude,
	flake::skydome::latitude const &_latitude)
:
	renderer_(
		_renderer),
	camera_(
		_camera),
	vertex_declaration_(
		renderer_.create_vertex_declaration(
			sge::renderer::vf::dynamic::make_format<flake::skydome::vf::format>())),
	vertex_buffer_(
		renderer_.create_vertex_buffer(
			*vertex_declaration_,
			sge::renderer::vf::dynamic::make_part_index
			<
				flake::skydome::vf::format,
				flake::skydome::vf::format_part
			>(),
			sge::renderer::vertex_count(
				1u + _latitude.get() * _longitude.get()),
			sge::renderer::resource_flags::none)),
	index_buffer_(
		renderer_.create_index_buffer(
			sge::renderer::index::dynamic::make_format<flake::skydome::index_format>(),
			sge::renderer::index_count(
				// For each pair of rings we have "it_lon" quads. We have
				// "it_lat-1" rings and "it_lat-2" pairs of rings.  This makes for
				// "it_lat-2" quads, each having 2 triangles and each triangle has
				// 3 indices.
				// the quads
				(_latitude.get()-1) * _longitude.get() * 2u * 3u +
				// the triangles
				_longitude.get() * 3u),
			sge::renderer::resource_flags::none)),
	shader_(
		_shader_context,
		*vertex_declaration_,
		flake::shader::vertex_program_path(
			flake::media_path_from_string(
				FCPPT_TEXT("shaders/skydome.cg"))),
		flake::shader::pixel_program_path(
			flake::media_path_from_string(
				FCPPT_TEXT("shaders/skydome.cg")))),
	mvp_parameter_(
		shader_.vertex_program(),
		flake::shader::parameter::name(
			sge::cg::string(
				"mvp")),
		flake::shader::parameter::is_projection_matrix(
			true),
		sge::renderer::matrix4())
{
	sge::renderer::scoped_vertex_lock vblock(
		*vertex_buffer_,
		sge::renderer::lock_mode::writeonly);

	sge::renderer::vf::view<flake::skydome::vf::format_part> vertices(
		vblock.value());

	sge::renderer::vf::view<flake::skydome::vf::format_part>::iterator vb_it(
		vertices.begin());

	sge::renderer::scalar const halfpi =
		fcppt::math::pi<sge::renderer::scalar>() /
		static_cast<sge::renderer::scalar>(
			2);

	sge::renderer::scalar const radius =
		1.0f;

	// The top
	vb_it->set<flake::skydome::vf::position>(
		::sphere_point(
			radius,
			static_cast<sge::renderer::scalar>(0),
			static_cast<sge::renderer::scalar>(0)));

	vb_it++;

	sge::renderer::scalar const
		increment_lat =
			halfpi /
			static_cast<sge::renderer::scalar>(
				_latitude.get()),
		increment_lon =
			fcppt::math::twopi<sge::renderer::scalar>()/
			static_cast<sge::renderer::scalar>(
				_longitude.get());

	sge::renderer::scalar lat =
		increment_lat;

	// We make one less iteration because we don't want the top
	// NOTE: We use size_type here FOR A REASON! floating point loops are inaccurate
	for (
		flake::skydome::size_type i =
			0u;
		i < _latitude.get();
		++i)
	{
		sge::renderer::scalar lon =
			static_cast<sge::renderer::scalar>(
				0);

		for(
			flake::skydome::size_type j = 0u;
			j < _longitude.get();
			++j)
		{
			(vb_it++)->set<flake::skydome::vf::position>(
				::sphere_point(
					radius,
					lat,
					lon));

			lon += increment_lon;
		}
		lat += increment_lat;
	}

	sge::renderer::scoped_index_lock const iblock(
		*index_buffer_,
		sge::renderer::lock_mode::writeonly);

	typedef
	sge::renderer::index::view<flake::skydome::index_format>
	index_view;

	index_view const view(
		iblock.value());

	index_view::iterator it(
		view.begin());

	// First, the triangles at the top
	for(
		flake::skydome::size_type lons =
			1u;
		lons <= _longitude.get();
		++lons)
	{
		(*it++).set(
			static_cast<index_view::value_type>(
				0u));
		(*it++).set(
			static_cast<index_view::value_type>(
				lons == _longitude.get() ? 1u : (lons+1u)));
		(*it++).set(
			static_cast<index_view::value_type>(
				lons));
	}

	// Start at the first vertex after the top
	index_view::value_type current =
		1u;

	// Again, we have it_lat-1 ring pairs (see below)
	for(
		flake::skydome::size_type lats =
			0u;
		lats < static_cast<flake::skydome::size_type>(_latitude.get()-1u);
		++lats)
	{
		// Remember the beginning index of the current circle so we can wrap around later in the inner loop
		index_view::value_type const beginning =
			current;

		// The last index would wrap around the first one, so it_lon-1
		for(
			flake::skydome::size_type lon =
				0u;
			lon < static_cast<flake::skydome::size_type>(_longitude.get());
			++lon)
		{
			index_view::value_type const
				next =
					lon == static_cast<size_type>(_longitude.get()-1u)
					?
						static_cast<index_view::value_type>(
							beginning)
					:
						static_cast<index_view::value_type>(
							current+1u),
				below =
					static_cast<index_view::value_type>(
						current + _longitude.get()),
				next_below =
					lon == static_cast<size_type>(_longitude.get()-1)
					?
						static_cast<index_view::value_type>(
							beginning + _longitude.get())
					:
						static_cast<index_view::value_type>(
							below+1);

			(*it++).set(
				current);

			(*it++).set(
				next_below);

			(*it++).set(
				below);

			(*it++).set(
				current);

			(*it++).set(
				next_below);

			(*it++).set(
				next);

			++current;
		}
	}
}

void
flake::skydome::object::render(
	sge::renderer::context::object &_context)
{
	sge::renderer::state::scoped scoped_state(
		_context,
		sge::renderer::state::list
			(sge::renderer::state::depth_func::off));

	sge::renderer::scoped_vertex_declaration scoped_vd(
		_context,
		*vertex_declaration_);

	flake::shader::scoped_pair scoped_shader(
		_context,
		shader_);

	sge::renderer::scoped_vertex_buffer scoped_vb(
		_context,
		*vertex_buffer_);

	mvp_parameter_.set(
		camera_.projection_matrix().get() *
		sge::camera::matrix_conversion::rotation(
			camera_.coordinate_system()));

	_context.render_indexed(
		*index_buffer_,
		sge::renderer::first_vertex(
			0u),
		sge::renderer::vertex_count(
			vertex_buffer_->size()),
		sge::renderer::primitive_type::triangle_list,
		sge::renderer::first_index(
			0u),
		sge::renderer::index_count(
			index_buffer_->size()));
}

flake::skydome::object::~object()
{
}
