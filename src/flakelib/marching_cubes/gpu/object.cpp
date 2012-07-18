#include <flakelib/buffer/volume_view_impl.hpp>
#include <flakelib/buffer_pool/volume_lock_impl.hpp>
#include <flakelib/marching_cubes/gpu/manager.hpp>
#include <flakelib/marching_cubes/gpu/object.hpp>
#include <flakelib/marching_cubes/vf/format.hpp>
#include <flakelib/scan/object.hpp>
#include <flakelib/volume/unique_uint_buffer_lock.hpp>
#include <sge/opencl/dim3.hpp>
#include <sge/opencl/size_type.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/command_queue/scoped_buffer_mapping.hpp>
#include <sge/opencl/memory_object/scoped_objects.hpp>
#include <sge/renderer/device.hpp>
#include <sge/renderer/resource_flags_none.hpp>
#include <sge/renderer/scoped_vertex_buffer.hpp>
#include <sge/renderer/vertex_buffer.hpp>
#include <sge/renderer/context/object.hpp>
#include <sge/renderer/vf/dynamic/make_part_index.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/assert/error.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/assign/make_container.hpp>
#include <fcppt/container/bitfield/object_impl.hpp>
#include <fcppt/math/log2.hpp>
#include <fcppt/math/dim/comparison.hpp>
#include <fcppt/math/dim/object_impl.hpp>
#include <fcppt/config/external_begin.hpp>
#include <cmath>
#include <fcppt/config/external_end.hpp>


flakelib::marching_cubes::gpu::object::object(
	flakelib::buffer_pool::object &_buffer_pool,
	flakelib::marching_cubes::gpu::manager &_manager,
	flakelib::marching_cubes::gpu::grid_size const &_grid_size,
	marching_cubes::iso_level const &_iso_level)
:
	manager_(
		_manager),
	buffer_pool_(
		_buffer_pool),
	command_queue_(
		_manager.command_queue_),
	grid_size_(
		_grid_size),
	grid_size_log2_(
		fcppt::math::log2(
			grid_size_.get().w()),
		fcppt::math::log2(
			grid_size_.get().h()),
		fcppt::math::log2(
			grid_size_.get().d()),
		0u),
	grid_size_mask_(
		flakelib::cl::uint4(
			static_cast<cl_uint>(
				grid_size_.get().w()-1u),
			static_cast<cl_uint>(
				grid_size_.get().h()-1u),
			static_cast<cl_uint>(
				grid_size_.get().d()-1u),
			0u)),
	grid_size_shift_(
		flakelib::cl::uint4(
			0u,
			grid_size_log2_[0],
			static_cast<cl_uint>(
				grid_size_log2_[0]+grid_size_log2_[1]),
			0u)),
	iso_level_(
		_iso_level),
	positions_buffer_(),
	normals_buffer_(),
	positions_buffer_cl_(),
	normals_buffer_cl_(),
	vertex_count_(
		0u)
{
	manager_.add_child(
		*this);
}

flakelib::marching_cubes::gpu::vertex_count const
flakelib::marching_cubes::gpu::object::update(
	flakelib::marching_cubes::gpu::density_view const &_density_view)
{
	FCPPT_ASSERT_PRE(
		_density_view.get().size().w() == grid_size_.get().w() &&
		_density_view.get().size().h() == grid_size_.get().h() &&
		_density_view.get().size().d() == grid_size_.get().d());

	sge::opencl::dim3 grid_size_dim(
		static_cast<sge::opencl::size_type>(
			grid_size_.get().w()),
		static_cast<sge::opencl::size_type>(
			grid_size_.get().h()),
		static_cast<sge::opencl::size_type>(
			grid_size_.get().d()));

	flakelib::volume::unique_uint_buffer_lock
		vertices_for_voxel(
			fcppt::make_unique_ptr<flakelib::volume::uint_buffer_lock>(
				fcppt::ref(
					buffer_pool_),
				grid_size_dim)),
		voxel_occupation(
			fcppt::make_unique_ptr<flakelib::volume::uint_buffer_lock>(
				fcppt::ref(
					buffer_pool_),
				grid_size_dim));

	manager_.classify_voxels(
		_density_view,
		flakelib::marching_cubes::gpu::vertices_for_voxel_view(
			vertices_for_voxel->value()),
		flakelib::marching_cubes::gpu::voxel_occupation_view(
			voxel_occupation->value()),
		iso_level_,
		grid_size_,
		grid_size_mask_,
		grid_size_shift_);

	flakelib::scan::object::unique_linear_uint_lock summed_voxel_occupation(
		manager_.scan().update(
			flakelib::scan::object::linear_uint_view(
				voxel_occupation->value().buffer()),
			flakelib::scan::batch_size(
				1u)));

	cl_uint const activeVoxels =
		this->sum_last_elements(
			summed_voxel_occupation->value(),
			flakelib::linear::uint_view(
				voxel_occupation->value().buffer()));

	if(activeVoxels == 0u)
		return
			flakelib::marching_cubes::gpu::vertex_count(
				0u);

	flakelib::volume::unique_uint_buffer_lock
		compacted_voxel_occupation(
			fcppt::make_unique_ptr<flakelib::volume::uint_buffer_lock>(
				fcppt::ref(
					buffer_pool_),
				grid_size_dim));

	manager_.compact_voxels(
		flakelib::marching_cubes::gpu::voxel_occupation_view(
			voxel_occupation->value()),
		flakelib::marching_cubes::gpu::summed_voxel_occupation_view(
			flakelib::volume::uint_view(
				summed_voxel_occupation->value().buffer(),
				grid_size_dim)),
		flakelib::marching_cubes::gpu::compacted_voxel_occupation_view(
			compacted_voxel_occupation->value()));


	// Not needed anymore
	summed_voxel_occupation.reset();
	voxel_occupation.reset();

	flakelib::scan::object::unique_linear_uint_lock summed_vertices_for_voxel(
		manager_.scan().update(
			flakelib::scan::object::linear_uint_view(
				vertices_for_voxel->value().buffer()),
			flakelib::scan::batch_size(
				1u)));

	// readback total number of vertices
	// The +3 here? I don't know :(
	vertex_count_ =
		this->sum_last_elements(
			summed_vertices_for_voxel->value(),
			flakelib::linear::uint_view(
				vertices_for_voxel->value().buffer()))+3u;

	// Not needed anymore
	vertices_for_voxel.reset();

	this->resize_gl_buffers();

	sge::opencl::memory_object::scoped_objects scoped_vb(
		command_queue_,
		fcppt::assign::make_container<sge::opencl::memory_object::base_ref_sequence>
			(positions_buffer_cl_.get())
			(normals_buffer_cl_.get()));

	manager_.generate_triangles(
		flakelib::marching_cubes::gpu::normals_buffer(
			*normals_buffer_cl_),
		flakelib::marching_cubes::gpu::positions_buffer(
			*positions_buffer_cl_),
		flakelib::marching_cubes::gpu::compacted_voxel_occupation_view(
				compacted_voxel_occupation->value()),
		flakelib::marching_cubes::gpu::summed_vertices_for_voxel_view(
			flakelib::volume::uint_view(
				summed_vertices_for_voxel->value().buffer(),
				grid_size_dim)),
		_density_view,
		grid_size_,
		grid_size_mask_,
		grid_size_shift_,
		iso_level_,
		flakelib::marching_cubes::gpu::active_voxels(
			activeVoxels),
		vertex_count_);

	return
		vertex_count_;
}

flakelib::marching_cubes::gpu::object::~object()
{
}

void
flakelib::marching_cubes::gpu::object::render(
	sge::renderer::context::object &_context)
{
	if(!positions_buffer_)
		return;

	FCPPT_ASSERT_PRE(
		normals_buffer_);

	FCPPT_ASSERT_PRE(
		positions_buffer_->size() == normals_buffer_->size());

	sge::renderer::scoped_vertex_buffer
		scoped_positions(
			_context,
			*positions_buffer_),
		scoped_normals(
			_context,
			*normals_buffer_);

	FCPPT_ASSERT_PRE(
		positions_buffer_->size().get() == normals_buffer_->size().get());

	FCPPT_ASSERT_PRE(
		vertex_count_.get() % 3u == 0u &&
		vertex_count_.get() <= positions_buffer_->size().get());

	_context.render_nonindexed(
		sge::renderer::first_vertex(
			0u),
		sge::renderer::vertex_count(
			static_cast<sge::renderer::size_type>(
				vertex_count_.get())),
		sge::renderer::primitive_type::triangle_list);
}

void
flakelib::marching_cubes::gpu::object::resize_gl_buffers()
{
	if(!vertex_count_.get())
		return;

	if(positions_buffer_ && vertex_count_.get() < positions_buffer_->size().get())
		return;

	sge::renderer::vertex_count const real_vertex_count(
		static_cast<sge::renderer::size_type>(
			2u * vertex_count_.get()));

	positions_buffer_.take(
		manager_.renderer().create_vertex_buffer(
			manager_.vertex_declaration(),
			sge::renderer::vf::dynamic::make_part_index
			<
				flakelib::marching_cubes::vf::format,
				flakelib::marching_cubes::vf::position_part
			>(),
			real_vertex_count,
			sge::renderer::resource_flags::none));

	normals_buffer_.take(
		manager_.renderer().create_vertex_buffer(
			manager_.vertex_declaration(),
			sge::renderer::vf::dynamic::make_part_index
			<
				flakelib::marching_cubes::vf::format,
				flakelib::marching_cubes::vf::normal_part
			>(),
			real_vertex_count,
			sge::renderer::resource_flags::none));

	positions_buffer_cl_.take(
		fcppt::make_unique_ptr<sge::opencl::memory_object::buffer>(
			fcppt::ref(
				command_queue_.context()),
			fcppt::ref(
				*positions_buffer_),
			sge::opencl::memory_object::renderer_buffer_lock_mode::read_write));

	normals_buffer_cl_.take(
		fcppt::make_unique_ptr<sge::opencl::memory_object::buffer>(
			fcppt::ref(
				command_queue_.context()),
			fcppt::ref(
				*normals_buffer_),
			sge::opencl::memory_object::renderer_buffer_lock_mode::read_write));
}

cl_uint
flakelib::marching_cubes::gpu::object::sum_last_elements(
	flakelib::linear::uint_view const &a,
	flakelib::linear::uint_view const &b)
{
	sge::opencl::command_queue::scoped_buffer_mapping a_mapping(
		command_queue_,
		a.buffer(),
		sge::opencl::command_queue::map_flags::read,
		sge::opencl::memory_object::byte_offset(
			static_cast<sge::opencl::memory_object::byte_offset::value_type>(
				(a.size().w()-1u) * sizeof(cl_uint))),
		sge::opencl::memory_object::byte_size(
			sizeof(cl_uint)),
		sge::opencl::event::sequence());

	sge::opencl::command_queue::scoped_buffer_mapping b_mapping(
		command_queue_,
		b.buffer(),
		sge::opencl::command_queue::map_flags::read,
		sge::opencl::memory_object::byte_offset(
			static_cast<sge::opencl::memory_object::byte_offset::value_type>(
				(b.size().w()-1u) * sizeof(cl_uint))),
		sge::opencl::memory_object::byte_size(
			sizeof(cl_uint)),
		sge::opencl::event::sequence());

	return
		*static_cast<cl_uint *>(
			a_mapping.ptr()) +
		*static_cast<cl_uint *>(
			b_mapping.ptr());
}
