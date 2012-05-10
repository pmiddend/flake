#include <flakelib/media_path_from_string.hpp>
#include <flakelib/cl/kernel.hpp>
#include <flakelib/cl/program_context.hpp>
#include <flakelib/scan/object.hpp>
#include <fcppt/insert_to_std_string.hpp>
#include <fcppt/make_unique_ptr.hpp>
#include <fcppt/move.hpp>
#include <fcppt/ref.hpp>
#include <fcppt/text.hpp>
#include <fcppt/assert/pre.hpp>
#include <fcppt/math/is_power_of_2.hpp>


namespace
{
//All three kernels run 512 threads per workgroup
//Must be a power of two
//const cl_uint WORKGROUP_SIZE = 256u;
const cl_uint WORKGROUP_SIZE = 512u;
//const char *compileOptions = "-D WORKGROUP_SIZE=256";

////////////////////////////////////////////////////////////////////////////////
// Common definitions
////////////////////////////////////////////////////////////////////////////////
const cl_uint MAX_BATCH_ELEMENTS = 64u * 1048576u;
const cl_uint MIN_SHORT_ARRAY_SIZE = 4u;
const cl_uint MAX_SHORT_ARRAY_SIZE = 4u * WORKGROUP_SIZE;
const cl_uint MIN_LARGE_ARRAY_SIZE = 8u * WORKGROUP_SIZE;
const cl_uint MAX_LARGE_ARRAY_SIZE = 4u * WORKGROUP_SIZE * WORKGROUP_SIZE;

cl_uint iSnapUp(cl_uint dividend, cl_uint divisor)
{
    return ((dividend % divisor) == 0) ? dividend : (dividend - dividend % divisor + divisor);
}
}

flakelib::scan::object::object(
	flakelib::cl::program_context const &_program_context,
	flakelib::buffer_pool::object &_buffer_pool)
:
	buffer_pool_(
		_buffer_pool),
	program_(
		_program_context.command_queue(),
		flakelib::media_path_from_string(
			FCPPT_TEXT("kernels/flakelib/scan.cl")),
		_program_context.compiler_flags() +
		flakelib::cl::compiler_flags(
			std::string(
				" -DWORKGROUP_SIZE=")+
			fcppt::insert_to_std_string(
				WORKGROUP_SIZE))),
	exclusive_local1_kernel_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"exclusive_local1"))),
	exclusive_local2_kernel_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"exclusive_local2"))),
	uniform_update_kernel_(
		program_.create_kernel(
			sge::opencl::kernel::name(
				"uniform_update"))),
	buffer_(
		fcppt::make_unique_ptr<linear_uint_lock>(
			fcppt::ref(
				buffer_pool_),
			sge::opencl::memory_object::dim1(
				static_cast<sge::opencl::memory_object::size_type>(
					(MAX_BATCH_ELEMENTS / (4u * WORKGROUP_SIZE)) * sizeof(cl_uint)))))
{
	exclusive_local2_kernel_->buffer_argument(
		"d_Buf",
		buffer_->value().buffer());

	uniform_update_kernel_->buffer_argument(
		"d_Buf",
		buffer_->value().buffer());
}

flakelib::scan::object::unique_linear_uint_lock
flakelib::scan::object::update(
	linear_uint_view const &_source,
	flakelib::scan::batch_size const &_batch_size)
{
	FCPPT_ASSERT_PRE(
		fcppt::math::is_power_of_2(
			_source.size().w()));

	// Check supported size range
	FCPPT_ASSERT_PRE(
	    (_source.size().w() >= MIN_LARGE_ARRAY_SIZE) && (_source.size().w() <= MAX_LARGE_ARRAY_SIZE));

	// Check total batch size limit
	FCPPT_ASSERT_PRE(
	    (_batch_size.get() * _source.size().w()) <= MAX_BATCH_ELEMENTS);

	unique_linear_uint_lock destination_buffer(
		fcppt::make_unique_ptr<linear_uint_lock>(
			fcppt::ref(
				buffer_pool_),
			_source.size()));

	this->exclusive_local1(
		source(
			_source),
		destination(
			destination_buffer->value()),
		workgroup_count(
			static_cast<cl_uint>(
				(_batch_size.get() * _source.size().w()) / (4u * WORKGROUP_SIZE))),
		elements_per_workgroup(
			4u * WORKGROUP_SIZE));

	this->exclusive_local2(
		source(
			_source),
		destination(
			destination_buffer->value()),
		_batch_size,
		block_count(
			static_cast<cl_uint>(
				_source.size().w() / (4u * WORKGROUP_SIZE))));

	this->uniform_update(
		destination(
			destination_buffer->value()),
		block_count(
			static_cast<cl_uint>(
				(_batch_size.get() * _source.size().w()) / (4u * WORKGROUP_SIZE))));

	return
		fcppt::move(
			destination_buffer);
}

flakelib::scan::object::~object()
{
}

void
flakelib::scan::object::exclusive_local1(
	source const &_source,
	destination const &_destination,
	workgroup_count const &_workgroup_count,
	elements_per_workgroup const &_elements_per_workgroup)
{
	exclusive_local1_kernel_->buffer_argument(
		"source",
		_source.get().buffer());

	exclusive_local1_kernel_->buffer_argument(
		"destination",
		_destination.get().buffer());

	exclusive_local1_kernel_->local_buffer_argument(
		"local_data",
		sge::opencl::memory_object::byte_size(
			2u * WORKGROUP_SIZE * sizeof(cl_uint)));

	exclusive_local1_kernel_->numerical_argument(
		"size",
		_elements_per_workgroup.get());

	exclusive_local1_kernel_->enqueue(
		flakelib::cl::global_dim1(
			static_cast<flakelib::cl::global_dim1::value_type>(
				(_workgroup_count.get() * _elements_per_workgroup.get()) / 4u)),
		flakelib::cl::local_dim1(
			static_cast<flakelib::cl::global_dim1::value_type>(
				WORKGROUP_SIZE)));
}

void
flakelib::scan::object::exclusive_local2(
	source const &_source,
	destination const &_destination,
	flakelib::scan::batch_size const &_batch_size,
	block_count const &_block_count)
{
	exclusive_local2_kernel_->buffer_argument(
		"d_Dst",
		_destination.get().buffer());

	exclusive_local2_kernel_->buffer_argument(
		"d_Src",
		_source.get().buffer());

	exclusive_local2_kernel_->local_buffer_argument(
		"l_Data",
		sge::opencl::memory_object::byte_size(
			2u * WORKGROUP_SIZE * sizeof(cl_uint)));

	exclusive_local2_kernel_->numerical_argument(
		"N",
		static_cast<cl_uint>(
			_batch_size.get() * _block_count.get()));

	exclusive_local2_kernel_->numerical_argument(
		"arrayLength",
		static_cast<cl_uint>(
			_block_count.get()));

	exclusive_local2_kernel_->enqueue(
		flakelib::cl::global_dim1(
			static_cast<flakelib::cl::global_dim1::value_type>(
				iSnapUp(
					_batch_size.get() * _block_count.get(),
					WORKGROUP_SIZE))),
		flakelib::cl::local_dim1(
			static_cast<flakelib::cl::global_dim1::value_type>(
				WORKGROUP_SIZE)));
}

void
flakelib::scan::object::uniform_update(
	destination const &_destination,
	block_count const &_block_count)
{
	uniform_update_kernel_->buffer_argument(
		"d_Data",
		_destination.get().buffer());

	uniform_update_kernel_->enqueue(
		flakelib::cl::global_dim1(
			static_cast<flakelib::cl::global_dim1::value_type>(
				_block_count.get() * WORKGROUP_SIZE)),
		flakelib::cl::local_dim1(
			static_cast<flakelib::cl::global_dim1::value_type>(
				WORKGROUP_SIZE)));
}
