#include <flakelib/buffer/volume_view.hpp>
#include <flakelib/marching_cubes/cpu/implementation.hpp>
#include <flakelib/marching_cubes/cpu/object.hpp>
#include <flakelib/marching_cubes/cpu/size_type.hpp>
#include <flakelib/marching_cubes/vf/interleaved.hpp>
#include <flakelib/marching_cubes/vf/interleaved_part.hpp>
#include <flakelib/timer/object.hpp>
#include <sge/opencl/command_queue/map_flags.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/command_queue/scoped_buffer_mapping.hpp>
#include <sge/opencl/event/object.hpp>
#include <sge/opencl/memory_object/buffer.hpp>
#include <sge/renderer/lock_mode.hpp>
#include <sge/renderer/primitive_type.hpp>
#include <sge/renderer/resource_flags_field.hpp>
#include <sge/renderer/context/core.hpp>
#include <sge/renderer/device/core.hpp>
#include <sge/renderer/index/buffer.hpp>
#include <sge/renderer/index/buffer_parameters.hpp>
#include <sge/renderer/index/format_32.hpp>
#include <sge/renderer/index/iterator.hpp>
#include <sge/renderer/index/scoped_lock.hpp>
#include <sge/renderer/index/view.hpp>
#include <sge/renderer/index/dynamic/make_format.hpp>
#include <sge/renderer/state/core/depth_stencil/object.hpp>
#include <sge/renderer/state/core/depth_stencil/parameters.hpp>
#include <sge/renderer/state/core/depth_stencil/scoped.hpp>
#include <sge/renderer/state/core/depth_stencil/depth/enabled.hpp>
#include <sge/renderer/state/core/depth_stencil/stencil/off.hpp>
#include <sge/renderer/vertex/buffer.hpp>
#include <sge/renderer/vertex/buffer_parameters.hpp>
#include <sge/renderer/vertex/declaration.hpp>
#include <sge/renderer/vertex/declaration_parameters.hpp>
#include <sge/renderer/vertex/scoped_buffer.hpp>
#include <sge/renderer/vertex/scoped_declaration.hpp>
#include <sge/renderer/vertex/scoped_lock.hpp>
#include <sge/renderer/vf/iterator.hpp>
#include <sge/renderer/vf/vertex.hpp>
#include <sge/renderer/vf/view.hpp>
#include <sge/renderer/vf/dynamic/make_format.hpp>
#include <sge/renderer/vf/dynamic/make_part_index.hpp>
#include <fcppt/insert_to_std_string.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/math/next_power_of_2.hpp>
#include <fcppt/math/dim/object_impl.hpp>
#include <fcppt/math/dim/output.hpp>
#include <fcppt/math/vector/object_impl.hpp>
#include <fcppt/config/external_begin.hpp>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <fcppt/config/external_end.hpp>


namespace
{
template<typename T>
T const &
volume_index(
	T const * const input,
	flakelib::marching_cubes::cpu::dim3 const &_position,
	flakelib::marching_cubes::cpu::grid_size const &_grid_size)
{
	return
		input[
			_grid_size.get().h() * _grid_size.get().w() * _position.d() +
			_grid_size.get().w() * _position.h() +
			_position.w()];
}

template<typename T>
T &
volume_index(
	T * const input,
	flakelib::marching_cubes::cpu::dim3 const &_position,
	flakelib::marching_cubes::cpu::grid_size const &_grid_size)
{
	return
		input[
			_grid_size.get().h() * _grid_size.get().w() * _position.d() +
			_grid_size.get().w() * _position.h() +
			_position.w()];
}

bool
filled_with_snow(
	flakelib::marching_cubes::cpu::scalar const _snow)
{
	flakelib::marching_cubes::cpu::scalar const
		snow_active_threshold =
			0.1f;

	return
		_snow > snow_active_threshold;
}

bool
filled_with_boundary(
	cl_float const _boundary)
{
	flakelib::marching_cubes::cpu::scalar const
		boundary_threshold =
			0.5f;

	return
		_boundary > boundary_threshold;
}

unsigned
stability_height(
	flakelib::marching_cubes::cpu::dim3 _position,
	flakelib::marching_cubes::cpu::scalar const * const _input,
	cl_float const * const _boundary,
	flakelib::marching_cubes::cpu::grid_size const &_grid_size)
{
	unsigned result =
		0u;

	flakelib::marching_cubes::cpu::scalar const
		snow_active_threshold =
			0.1f,
		boundary_threshold =
			0.5f;

	while(
		_position.h() != 0u &&
		volume_index(_input,_position,_grid_size) < snow_active_threshold &&
		volume_index(_boundary,_position,_grid_size) < boundary_threshold)
	{
		result++;
		_position.h() -= 1u;
	}

	return
		result;
}

flakelib::marching_cubes::cpu::dim3 const
determine_deposit_place(
	flakelib::marching_cubes::cpu::dim3 _position,
	flakelib::marching_cubes::cpu::scalar const * const _input,
	cl_float const * const _boundary,
	flakelib::marching_cubes::cpu::grid_size const &_grid_size)
{
	while(
		// Entweder am Boden angekommen
		_position.h() != 0u &&
		// Oder das Feld dadrunter ist mit Schnee gefüllt
		!filled_with_snow(
			volume_index(
				_input,
				flakelib::marching_cubes::cpu::dim3(
					_position.w(),
					_position.h()-1u,
					_position.d()),
				_grid_size)) &&
		// Oder das Feld dadrunter ist mit einem Hindernis gefüllt
		!filled_with_boundary(
			volume_index(
				_boundary,
				flakelib::marching_cubes::cpu::dim3(
					_position.w(),
					_position.h()-1u,
					_position.d()),
				_grid_size)))
		_position.h() -= 1u;

	return
		_position;
}

void
apply_fall_stability_condition_single_cell(
	flakelib::marching_cubes::cpu::dim3 const &_position,
	flakelib::marching_cubes::cpu::scalar * const _snow_density,
	cl_float const * const _boundary,
	flakelib::marching_cubes::cpu::grid_size const &_grid_size)
{
	flakelib::marching_cubes::cpu::dim3 const bottom_position(
		_position.w(),
		_position.h()-1u,
		_position.d());

	flakelib::marching_cubes::cpu::scalar &bottom_snow_value =
		volume_index(
			_snow_density,
			bottom_position,
			_grid_size);

	flakelib::marching_cubes::cpu::scalar &current_snow_value =
		volume_index(
			_snow_density,
			_position,
			_grid_size);

	flakelib::marching_cubes::cpu::scalar const snow_threshold =
		0.01f;

	if(
		current_snow_value > snow_threshold &&
		!filled_with_snow(
			bottom_snow_value) &&
		!filled_with_boundary(
			volume_index(
				_boundary,
				bottom_position,
				_grid_size)))
	{
		flakelib::marching_cubes::cpu::dim3 const deposit_position(
			determine_deposit_place(
				_position,
				_snow_density,
				_boundary,
				_grid_size));

		/*
		if(deposit_position.h() != 0u)
			std::cout << deposit_position << "\n";
		*/

		flakelib::marching_cubes::cpu::scalar &deposit_snow_value =
			volume_index(
				_snow_density,
				deposit_position,
				_grid_size);

//		std::cout << "Depositing " << current_snow_value << " from " <<  _position << " to " << deposit_position << "\n";

		deposit_snow_value += current_snow_value;
		current_snow_value = 0.0f;
	}
}

void
apply_erosion_stability_condition_single_cell(
	flakelib::marching_cubes::cpu::dim3 const &_position,
	flakelib::marching_cubes::cpu::scalar * const _snow_density,
	cl_float const * const _boundary,
	flakelib::marching_cubes::cpu::grid_size const &_grid_size)
{
	unsigned const steepness =
		3u;

	flakelib::marching_cubes::cpu::scalar &current_snow_value =
		volume_index(
			_snow_density,
			_position,
			_grid_size);

	if(!filled_with_snow(current_snow_value))
		return;

	flakelib::marching_cubes::cpu::scalar const neighbor_fractions =
		1.0f;

	// Has right neighbor
	if(_position.w() < _grid_size.get().w()-1u)
	{
		flakelib::marching_cubes::cpu::dim3 const neighbor_position(
			_position.w()+1u,
			_position.h(),
			_position.d());

		flakelib::marching_cubes::cpu::dim3 const deposit_position(
			determine_deposit_place(
				neighbor_position,
				_snow_density,
				_boundary,
				_grid_size));

		flakelib::marching_cubes::cpu::size_type const height =
			neighbor_position.h() - deposit_position.h();

		if(height > steepness)
		{
			flakelib::marching_cubes::cpu::scalar &deposit_snow_value =
				volume_index(
					_snow_density,
					deposit_position,
					_grid_size);

			deposit_snow_value += current_snow_value * neighbor_fractions;
			current_snow_value -= current_snow_value * neighbor_fractions;
		}
	}

	// Has left neighbor
	if(_position.w() > 0u)
	{
		flakelib::marching_cubes::cpu::dim3 const neighbor_position(
			_position.w()-1u,
			_position.h(),
			_position.d());

		flakelib::marching_cubes::cpu::dim3 const deposit_position(
			determine_deposit_place(
				neighbor_position,
				_snow_density,
				_boundary,
				_grid_size));

		flakelib::marching_cubes::cpu::size_type const height =
			neighbor_position.h() - deposit_position.h();

		if(height > steepness)
		{
			flakelib::marching_cubes::cpu::scalar &deposit_snow_value =
				volume_index(
					_snow_density,
					deposit_position,
					_grid_size);

			deposit_snow_value += current_snow_value * neighbor_fractions;
			current_snow_value -= current_snow_value * neighbor_fractions;
		}
	}

	// Has backward neighbor
	if(_position.d() > 0u)
	{
		flakelib::marching_cubes::cpu::dim3 const neighbor_position(
			_position.w(),
			_position.h(),
			_position.d()-1u);

		flakelib::marching_cubes::cpu::dim3 const deposit_position(
			determine_deposit_place(
				neighbor_position,
				_snow_density,
				_boundary,
				_grid_size));

		flakelib::marching_cubes::cpu::size_type const height =
			neighbor_position.h() - deposit_position.h();

		if(height > steepness)
		{
			flakelib::marching_cubes::cpu::scalar &deposit_snow_value =
				volume_index(
					_snow_density,
					deposit_position,
					_grid_size);

			deposit_snow_value += current_snow_value * neighbor_fractions;
			current_snow_value -= current_snow_value * neighbor_fractions;
		}
	}

	// Has right neighbor
	if(_position.d() < _grid_size.get().d()-1u)
	{
		flakelib::marching_cubes::cpu::dim3 const neighbor_position(
			_position.w(),
			_position.h(),
			_position.d()+1u);

		flakelib::marching_cubes::cpu::dim3 const deposit_position(
			determine_deposit_place(
				neighbor_position,
				_snow_density,
				_boundary,
				_grid_size));

		flakelib::marching_cubes::cpu::size_type const height =
			neighbor_position.h() - deposit_position.h();

		if(height > steepness)
		{
			flakelib::marching_cubes::cpu::scalar &deposit_snow_value =
				volume_index(
					_snow_density,
					deposit_position,
					_grid_size);

			deposit_snow_value += current_snow_value * neighbor_fractions;
			current_snow_value -= current_snow_value * neighbor_fractions;
		}
	}
}

void
apply_stability_conditions_whole_view(
	flakelib::marching_cubes::cpu::scalar * const _input,
	cl_float const * const _boundary,
	flakelib::marching_cubes::cpu::grid_size const &_grid_size)
{
	for(flakelib::marching_cubes::cpu::size_type y = 1u; y < _grid_size.get().h(); ++y)
	{
		for(flakelib::marching_cubes::cpu::size_type x = 0u; x < _grid_size.get().w(); ++x)
		{
			for(flakelib::marching_cubes::cpu::size_type z = 0u; z < _grid_size.get().d(); ++z)
			{
				apply_fall_stability_condition_single_cell(
					flakelib::marching_cubes::cpu::dim3(
						x,
						y,
						z),
					_input,
					_boundary,
					_grid_size);
			}
		}
	}

	for(flakelib::marching_cubes::cpu::size_type y = _grid_size.get().h(); y > 0u; --y)
	{
		for(flakelib::marching_cubes::cpu::size_type x = 0u; x < _grid_size.get().w(); ++x)
		{
			for(flakelib::marching_cubes::cpu::size_type z = 0u; z < _grid_size.get().d(); ++z)
			{
				apply_erosion_stability_condition_single_cell(
					flakelib::marching_cubes::cpu::dim3(
						x,
						y,
						z),
					_input,
					_boundary,
					_grid_size);
			}
		}
	}
}
}

flakelib::marching_cubes::cpu::object::object(
	sge::renderer::device::core &_renderer,
	sge::opencl::command_queue::object &_command_queue,
	flakelib::marching_cubes::cpu::grid_size const &_grid_size,
	flakelib::marching_cubes::iso_level const &_iso_level,
	flakelib::volume::boundary_buffer_view const &_boundary)
:
	renderer_(
		_renderer),
	grid_size_(
		_grid_size),
	iso_level_(
		_iso_level),
	vertex_declaration_(
		renderer_.create_vertex_declaration(
			sge::renderer::vertex::declaration_parameters(
				sge::renderer::vf::dynamic::make_format<flakelib::marching_cubes::vf::interleaved>()))),
	vertex_buffer_(),
	vertex_buffer_size_(
		0u),
	index_buffer_(),
	index_buffer_size_(
		0u),
	depth_stencil_state_(
		renderer_.create_depth_stencil_state(
			sge::renderer::state::core::depth_stencil::parameters(
				sge::renderer::state::core::depth_stencil::depth::variant(
					sge::renderer::state::core::depth_stencil::depth::enabled(
						sge::renderer::state::core::depth_stencil::depth::func::less,
						sge::renderer::state::core::depth_stencil::depth::write_enable(
							true))),
				sge::renderer::state::core::depth_stencil::stencil::off()))),
	implementation_(
		fcppt::make_unique_ptr<MarchingCubes>(
			static_cast<int>(
				grid_size_.get().w()),
			static_cast<int>(
				grid_size_.get().h()),
			static_cast<int>(
				grid_size_.get().d()))),
	data_(
		static_cast<real_sequence::size_type>(
			_grid_size.get().content())),
	vertex_buffer_data_(),
	index_data_(),
	is_dirty_(
		false),
	boundary_(
		static_cast<cl_float_sequence::size_type>(
			_boundary.get().size().content()))
{
	implementation_->set_ext_data(
		data_.data());

	sge::opencl::command_queue::scoped_buffer_mapping buffer_mapping(
		_command_queue,
		_boundary.get().buffer(),
		sge::opencl::command_queue::map_flags::read,
		sge::opencl::memory_object::byte_offset(
			0u),
		_boundary.get().buffer().byte_size(),
		sge::opencl::event::sequence());

	std::copy(
		static_cast<cl_float const *>(
			buffer_mapping.ptr()),
		static_cast<cl_float const *>(
			buffer_mapping.ptr()) + _boundary.get().size().content(),
		boundary_.begin());
}

void
flakelib::marching_cubes::cpu::object::render(
	sge::renderer::context::core &_context)
{
	if(!vertex_buffer_size_.get())
	{
		FCPPT_ASSERT_PRE(
			!index_buffer_size_.get());
		return;
	}

	FCPPT_ASSERT_PRE(
		index_buffer_);

	sge::renderer::state::core::depth_stencil::scoped const depth_transform(
		_context,
		*depth_stencil_state_);

	sge::renderer::vertex::scoped_declaration scoped_vertex_declaration(
		_context,
		*vertex_declaration_);

	sge::renderer::vertex::scoped_buffer scoped_vertex_buffer(
		_context,
		*vertex_buffer_);

	_context.render_indexed(
		*index_buffer_,
		sge::renderer::vertex::first(
			0u),
		vertex_buffer_size_,
		sge::renderer::primitive_type::triangle_list,
		sge::renderer::index::first(
			0u),
		index_buffer_size_);
}

void
flakelib::marching_cubes::cpu::object::construct_from_raw_data(
	sge::renderer::scalar const *_data)
{
	is_dirty_ =
		true;

	/* DEBUG OUTPUT
	std::cout << "construct from raw data (data size is " << data_.size() << ")\n";
	for(sge::renderer::scalar const *it = _data; it != _data + data_.size(); ++it)
	{
		if((it - _data) % 64 == 0)
			std::cout << "\n";
		if((it - _data) % (64*64) == 0)
		{
			static unsigned slice_count = 1u;
			std::cout << "slice " << ++slice_count << "\n";
			std::cout << "\n";
		}
		std::cout << ((*it > 0.5f) ? 1 : 0);
	}
	*/
	{
		/*
		flakelib::timer::object t(
			std::cout,
			"copying float[] -> double[]");
		*/

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
	flakelib::timer::object t(
		std::cout,
		"marching_cubes::construction");

	#if 0
	sge::opencl::command_queue::scoped_buffer_mapping buffer_mapping(
		_command_queue,
		_view.buffer(),
		sge::opencl::command_queue::map_flags::read,
		sge::opencl::memory_object::byte_offset(
			0u),
		_view.buffer().byte_size(),
		sge::opencl::event::sequence());

	this->construct_from_raw_data(
		static_cast<cl_float const *>(
			buffer_mapping.ptr()));
	#endif
	#if 1
	is_dirty_ = true;

	clEnqueueReadBuffer(
		_command_queue.impl(),
		_view.buffer().impl(),
		CL_TRUE,
		0,
		_view.buffer().byte_size().get(),
		data_.data(),
		0,
		0,
		0);
	#endif
}

void
flakelib::marching_cubes::cpu::object::run()
{
	/*
	std::cout << "Applying stability conditions\n";
	apply_stability_conditions_whole_view(
		data_.data(),
		boundary_.data(),
		grid_size_);
	*/

	implementation_->clean_all();
	implementation_->set_resolution(
		static_cast<int>(
			grid_size_.get().w()),
		static_cast<int>(
			grid_size_.get().h()),
		static_cast<int>(
			grid_size_.get().d()));
	implementation_->init_all();

	//flakelib::timer::object t(std::cout,"marching_cubes::run");
	implementation_->run(
		static_cast<real_sequence::value_type>(
			iso_level_.get()));

	is_dirty_ =
		false;

	{
		vertex_buffer_data_.resize(
			static_cast<renderer_scalar_sequence::size_type>(
				implementation_->nverts()) *
			static_cast<renderer_scalar_sequence::size_type>(
				8u));

		renderer_scalar_sequence::iterator it =
			vertex_buffer_data_.begin();

		for(
			Vertex const *v =
				implementation_->vertices();
			v != implementation_->vertices() + implementation_->nverts();
			++v)
		{
			*it++ = static_cast<sge::renderer::scalar>(v->x);
			*it++ = static_cast<sge::renderer::scalar>(v->y);
			*it++ = static_cast<sge::renderer::scalar>(v->z);
			*it++ = static_cast<sge::renderer::scalar>(1.0f);

			*it++ = static_cast<sge::renderer::scalar>(v->nx);
			*it++ = static_cast<sge::renderer::scalar>(v->ny);
			*it++ = static_cast<sge::renderer::scalar>(v->nz);
			*it++ = static_cast<sge::renderer::scalar>(1.0f);
		}
	}

	{
		index_data_.resize(
			static_cast<sge::renderer::size_type>(
				implementation_->ntrigs()) *
			3u);

		index_sequence::iterator it =
			index_data_.begin();

		for(
			Triangle *t = implementation_->triangles();
			t != implementation_->triangles() + implementation_->ntrigs();
			++t)
		{
			*it++ =
				static_cast<index_sequence::value_type>(
					t->v1);
			*it++ =
				static_cast<index_sequence::value_type>(
					t->v2);
			*it++ =
				static_cast<index_sequence::value_type>(
					t->v3);
			/*
			(*it++).set(
				static_cast<index_view::value_type>(
					t->v1));
			(*it++).set(
				static_cast<index_view::value_type>(
					t->v2));
			(*it++).set(
				static_cast<index_view::value_type>(
					t->v3));
			*/
		}
	}
}

void
flakelib::marching_cubes::cpu::object::update_buffers()
{
	if(!implementation_->ntrigs())
		return;

	flakelib::timer::object t(std::cout,"marching_cubes::buffer fill total");
	this->fill_vertex_buffer();
	this->fill_index_buffer();
}

sge::renderer::vertex::declaration &
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
	vertex_buffer_size_ =
		sge::renderer::vertex::count(
			static_cast<sge::renderer::size_type>(implementation_->nverts()));

	if(
		!vertex_buffer_ ||
		vertex_buffer_->size() < vertex_buffer_size_)
	{
		sge::renderer::vertex::count const vertex_buffer_capacity(
			fcppt::math::next_power_of_2(
				vertex_buffer_size_.get()));

		std::cout << "Reallocating vertex buffer (size " << vertex_buffer_size_.get() << ", capacity " << vertex_buffer_capacity.get() << ")\n";

		vertex_buffer_ =
			renderer_.create_vertex_buffer(
				sge::renderer::vertex::buffer_parameters(
					*vertex_declaration_,
					sge::renderer::vf::dynamic::make_part_index
					<
						flakelib::marching_cubes::vf::interleaved,
						flakelib::marching_cubes::vf::interleaved_part
					>(),
					vertex_buffer_capacity,
					sge::renderer::resource_flags_field::null()));
	}

	sge::renderer::vertex::scoped_lock vblock(
		*vertex_buffer_,
		sge::renderer::lock_mode::writeonly);

	/*
	FCPPT_ASSERT_PRE(
		vblock.value().size().get() ==
		vertex_buffer_data_.size() * sizeof(renderer_scalar_sequence::value_type));
	*/

	/*
	flakelib::timer::object t(
		std::cout,
		"marching_cubes::vertex buffer fill: "+
		fcppt::insert_to_std_string(
			vblock.value().size().get() / 1024));
	*/

	std::cout << "Buffer size: " << vertex_buffer_data_.size() << ", vbsize value: " << vertex_buffer_size_.get() << "\n";
	std::memcpy(
		vblock.value().data(),
		reinterpret_cast<sge::renderer::raw_pointer>(
			vertex_buffer_data_.data()),
		vertex_buffer_size_.get() * 8u * sizeof(sge::renderer::scalar));

	/*
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
	*/
}

void
flakelib::marching_cubes::cpu::object::fill_index_buffer()
{
	if(!implementation_->ntrigs())
		return;

	typedef
	sge::renderer::index::format_32
	index_format;

	index_buffer_size_ =
		sge::renderer::index::count(
			static_cast<sge::renderer::size_type>(
				implementation_->ntrigs()) *
			3u);

	if(
		!index_buffer_ ||
		index_buffer_->size() < index_buffer_size_)
	{
		sge::renderer::index::count const index_buffer_capacity(
			fcppt::math::next_power_of_2(
				index_buffer_size_.get()));

		std::cerr << "Reallocating index buffer (size " << index_buffer_size_.get() << ", capacity " << index_buffer_capacity.get() << ")\n";

		index_buffer_ =
			renderer_.create_index_buffer(
				sge::renderer::index::buffer_parameters(
					sge::renderer::index::dynamic::make_format<index_format>(),
					index_buffer_capacity,
					sge::renderer::resource_flags_field::null()));
	}

	/*
	flakelib::timer::object index_timer(
		std::cout,
		"marching_cubes::index buffer fill: "+
		fcppt::insert_to_std_string(
			(static_cast<sge::renderer::size_type>(implementation_->ntrigs()) * 3u * 4u)/1024u));
	*/

	sge::renderer::index::scoped_lock const iblock(
		*index_buffer_,
		sge::renderer::lock_mode::writeonly);

	FCPPT_ASSERT_PRE(
		index_buffer_size_.get() == index_data_.size());

	#if 1
	std::memcpy(
		iblock.value().data(),
		reinterpret_cast<sge::renderer::raw_pointer>(
			index_data_.data()),
		index_buffer_size_.get() * sizeof(index_sequence::value_type));
	#endif

	#if 0
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
	#endif
}
