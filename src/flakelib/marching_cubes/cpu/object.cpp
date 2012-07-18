#include <flakelib/buffer/volume_view.hpp>
#include <flakelib/marching_cubes/cpu/implementation.hpp>
#include <flakelib/marching_cubes/cpu/object.hpp>
#include <flakelib/marching_cubes/vf/interleaved.hpp>
#include <flakelib/marching_cubes/vf/interleaved_part.hpp>
#include <flakelib/timer/object.hpp>
#include <sge/opencl/command_queue/scoped_buffer_mapping.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/index_buffer.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/scoped_index_lock.hpp>
#include <sge/renderer/scoped_vertex_buffer.hpp>
#include <sge/renderer/scoped_vertex_declaration.hpp>
#include <sge/renderer/scoped_vertex_lock.hpp>
#include <sge/renderer/vertex_buffer.hpp>
#include <sge/renderer/vertex_declaration.hpp>
#include <sge/renderer/context/object.hpp>
#include <sge/renderer/index/format_32.hpp>
#include <sge/renderer/index/iterator.hpp>
#include <sge/renderer/index/view.hpp>
#include <sge/renderer/index/dynamic/make_format.hpp>
#include <sge/renderer/state/cull_mode.hpp>
#include <sge/renderer/state/depth_func.hpp>
#include <sge/renderer/state/list.hpp>
#include <sge/renderer/state/scoped.hpp>
#include <sge/renderer/vf/iterator.hpp>
#include <sge/renderer/vf/vertex.hpp>
#include <sge/renderer/vf/view.hpp>
#include <sge/renderer/vf/dynamic/make_format.hpp>
#include <sge/renderer/vf/dynamic/make_part_index.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/math/dim/object_impl.hpp>
#include <fcppt/config/external_begin.hpp>
#include <algorithm>
#include <iostream>
#include <fcppt/config/external_end.hpp>


flakelib::marching_cubes::cpu::object::object(
	sge::renderer::device &_renderer,
	flakelib::marching_cubes::cpu::grid_size const &_grid_size,
	flakelib::marching_cubes::iso_level const &_iso_level)
:
	renderer_(
		_renderer),
	iso_level_(
		_iso_level),
	vertex_declaration_(
		renderer_.create_vertex_declaration(
			sge::renderer::vf::dynamic::make_format<flakelib::marching_cubes::vf::interleaved>())),
	vertex_buffer_(),
	index_buffer_(),
	implementation_(
		fcppt::make_unique_ptr<MarchingCubes>(
			static_cast<int>(
				_grid_size.get().w()),
			static_cast<int>(
				_grid_size.get().h()),
			static_cast<int>(
				_grid_size.get().d()))),
	data_(
		static_cast<real_sequence::size_type>(
			_grid_size.get().content())),
	is_dirty_(
		false)
{
	implementation_->set_ext_data(
		data_.data());
	implementation_->init_all();
}

void
flakelib::marching_cubes::cpu::object::render(
	sge::renderer::context::object &_context)
{
	if(!vertex_buffer_)
	{
		FCPPT_ASSERT_PRE(
			!index_buffer_);
		return;
	}

	FCPPT_ASSERT_PRE(
		index_buffer_);

	sge::renderer::state::scoped scoped_state(
		_context,
		sge::renderer::state::list
			(sge::renderer::state::cull_mode::off)
			(sge::renderer::state::depth_func::less));

	sge::renderer::scoped_vertex_declaration scoped_vertex_declaration(
		_context,
		*vertex_declaration_);

	sge::renderer::scoped_vertex_buffer scoped_vertex_buffer(
		_context,
		*vertex_buffer_);

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

void
flakelib::marching_cubes::cpu::object::construct_from_raw_data(
	sge::renderer::scalar const *_data)
{
	is_dirty_ =
		true;

	{
		//		flakelib::timer::object t(std::cout,"copying float[] -> double[]");

		std::copy(
			_data,
			_data + data_.size(),
			data_.begin());
	}
}

void
flakelib::marching_cubes::cpu::object::construct_from_cl_buffer(
	sge::opencl::command_queue::object &_command_queue,
	flakelib::volume::float_view const &_view)
{
	//	flakelib::timer::object t(std::cout,"construction total");

	sge::opencl::command_queue::scoped_buffer_mapping buffer_mapping(
		_command_queue,
		_view.buffer(),
		sge::opencl::command_queue::map_flags::read,
		sge::opencl::memory_object::byte_offset(
			0u),
		_view.buffer().byte_size(),
		sge::opencl::event::sequence());

	this->construct_from_raw_data(
		static_cast<sge::renderer::scalar const *>(
			buffer_mapping.ptr()));
}

void
flakelib::marching_cubes::cpu::object::run()
{
	//	flakelib::timer::object t(std::cout,"marching cubes");
	implementation_->run(
		static_cast<real_sequence::value_type>(
			iso_level_.get()));
	is_dirty_ =
		false;
}

void
flakelib::marching_cubes::cpu::object::update_buffers()
{
	if(!implementation_->ntrigs())
		return;

	//	flakelib::timer::object t(std::cout,"buffer fill");
	this->fill_vertex_buffer();
	this->fill_index_buffer();
}

sge::renderer::vertex_declaration &
flakelib::marching_cubes::cpu::object::vertex_declaration()
{
	return
		*vertex_declaration_;
}

bool
flakelib::marching_cubes::cpu::object::is_dirty() const
{
	return
		is_dirty_;
}

flakelib::marching_cubes::cpu::object::~object()
{
	implementation_->clean_all();
}

void
flakelib::marching_cubes::cpu::object::fill_vertex_buffer()
{
	std::cout << "Vertices: " << implementation_->nverts() << "\n";
	vertex_buffer_.take(
		renderer_.create_vertex_buffer(
			*vertex_declaration_,
			sge::renderer::vf::dynamic::make_part_index
			<
				flakelib::marching_cubes::vf::interleaved,
				flakelib::marching_cubes::vf::interleaved_part
			>(),
			sge::renderer::vertex_count(
				static_cast<sge::renderer::size_type>(
					implementation_->nverts())),
			sge::renderer::resource_flags::none));

	sge::renderer::scoped_vertex_lock vblock(
		*vertex_buffer_,
		sge::renderer::lock_mode::writeonly);

	sge::renderer::vf::view<flakelib::marching_cubes::vf::interleaved_part> vertices(
		vblock.value());

	sge::renderer::vf::view<flakelib::marching_cubes::vf::interleaved_part>::iterator vb_it(
		vertices.begin());

	for(
		Vertex const *v =
			implementation_->vertices();
		v != implementation_->vertices() + implementation_->nverts();
		++v)
	{
		vb_it->set<flakelib::marching_cubes::vf::position>(
			flakelib::marching_cubes::vf::position::packed_type(
				static_cast<sge::renderer::scalar>(
					v->x),
				static_cast<sge::renderer::scalar>(
					v->y),
				static_cast<sge::renderer::scalar>(
					v->z),
				static_cast<sge::renderer::scalar>(
					1.0f)));

		(vb_it++)->set<flakelib::marching_cubes::vf::normal>(
			flakelib::marching_cubes::vf::normal::packed_type(
				static_cast<sge::renderer::scalar>(
					v->nx),
				static_cast<sge::renderer::scalar>(
					v->ny),
				static_cast<sge::renderer::scalar>(
					v->nz),
				static_cast<sge::renderer::scalar>(
					1.0f)));
	}
}

void
flakelib::marching_cubes::cpu::object::fill_index_buffer()
{
	typedef
	sge::renderer::index::format_32
	index_format;

	index_buffer_.take(
		renderer_.create_index_buffer(
			sge::renderer::index::dynamic::make_format<index_format>(),
			sge::renderer::index_count(
				static_cast<sge::renderer::size_type>(
					implementation_->ntrigs()) *
				3u),
			sge::renderer::resource_flags::none));

	sge::renderer::scoped_index_lock const iblock(
		*index_buffer_,
		sge::renderer::lock_mode::writeonly);

	typedef
	sge::renderer::index::view<index_format>
	index_view;

	index_view const view(
		iblock.value());

	index_view::iterator it(
		view.begin());

	for(
		Triangle *t = implementation_->triangles();
		t != implementation_->triangles() + implementation_->ntrigs();
		++t)
	{
		(*it++).set(
			static_cast<index_view::value_type>(
				t->v1));
		(*it++).set(
			static_cast<index_view::value_type>(
				t->v2));
		(*it++).set(
			static_cast<index_view::value_type>(
				t->v3));
	}
}
