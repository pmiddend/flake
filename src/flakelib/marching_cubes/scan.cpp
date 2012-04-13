#include <flakelib/media_path_from_string.hpp>
#include <flakelib/cl/program_context.hpp>
#include <flakelib/marching_cubes/scan.hpp>
#include <sge/opencl/clinclude.hpp>
#include <sge/opencl/command_queue/object.hpp>
#include <sge/opencl/program/build_parameters.hpp>
#include <sge/opencl/program/file_to_source_string_sequence.hpp>
#include <fcppt/assert/error.hpp>
#include <fcppt/container/bitfield/object_impl.hpp>


namespace
{
//All three kernels run 512 threads per workgroup
//Must be a power of two
const cl_uint WORKGROUP_SIZE = 256u;
const char *compileOptions = "-D WORKGROUP_SIZE=256";

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

cl_uint factorRadix2(cl_uint& log2L, cl_uint L)
{
    if(!L){
        log2L = 0;
        return 0;
    }
    else
    {
        for(log2L = 0; (L & 1) == 0; L >>= 1, log2L++);
        return L;
    }
}
}

flakelib::marching_cubes::scan::scan(
	flakelib::cl::program_context const &_program_context)
:
	command_queue_(
		_program_context.command_queue()),
	program_(
		command_queue_.context(),
		sge::opencl::program::file_to_source_string_sequence(
			flakelib::media_path_from_string(
				FCPPT_TEXT("kernels/marching_cubes/scan.cl"))),
		sge::opencl::program::optional_build_parameters(
			sge::opencl::program::build_parameters()
				.options(
					_program_context.compiler_flags().get()))),
	ckScanExclusiveLocal1(
		program_,
		sge::opencl::kernel::name(
			"scanExclusiveLocal1")),
	ckScanExclusiveLocal2(
		program_,
		sge::opencl::kernel::name(
			"scanExclusiveLocal2")),
	ckUniformUpdate(
		program_,
		sge::opencl::kernel::name(
			"uniformUpdate")),
	buffer_(
		command_queue_.context(),
		sge::opencl::memory_object::flags_field(
			sge::opencl::memory_object::flags::read) |
		sge::opencl::memory_object::flags::write,
		sge::opencl::memory_object::byte_size(
			(MAX_BATCH_ELEMENTS / (4u * WORKGROUP_SIZE)) * sizeof(cl_uint)))
{
}

flakelib::marching_cubes::scan::~scan()
{
}

////////////////////////////////////////////////////////////////////////////////
// Short scan launcher
////////////////////////////////////////////////////////////////////////////////
std::size_t
flakelib::marching_cubes::scan::scanExclusiveLocal1(
    cl_mem d_Dst,
    cl_mem d_Src,
    cl_uint n,
    cl_uint size)
{
    cl_int ciErrNum;
    size_t localWorkSize, globalWorkSize;

    ciErrNum  = clSetKernelArg(ckScanExclusiveLocal1.impl(), 0, sizeof(cl_mem), (void *)&d_Dst);
    ciErrNum |= clSetKernelArg(ckScanExclusiveLocal1.impl(), 1, sizeof(cl_mem), (void *)&d_Src);
    ciErrNum |= clSetKernelArg(ckScanExclusiveLocal1.impl(), 2, 2 * WORKGROUP_SIZE * sizeof(cl_uint), NULL);
    ciErrNum |= clSetKernelArg(ckScanExclusiveLocal1.impl(), 3, sizeof(cl_uint), (void *)&size);
    FCPPT_ASSERT_ERROR(
	    ciErrNum == CL_SUCCESS);

    localWorkSize = WORKGROUP_SIZE;
    globalWorkSize = (n * size) / 4;

    ciErrNum = clEnqueueNDRangeKernel(command_queue_.impl(), ckScanExclusiveLocal1.impl(), 1, NULL, &globalWorkSize, &localWorkSize, 0, NULL, NULL);

    FCPPT_ASSERT_ERROR(
	    ciErrNum == CL_SUCCESS);

    return localWorkSize;
}

std::size_t
flakelib::marching_cubes::scan::scanExclusiveShort(
    cl_mem d_Dst,
    cl_mem d_Src,
    cl_uint batchSize,
    cl_uint arrayLength)
{
    //Check power-of-two factorization
    cl_uint log2L;
    cl_uint factorizationRemainder = factorRadix2(log2L, arrayLength);

    FCPPT_ASSERT_ERROR(
	    factorizationRemainder == 1);

    FCPPT_ASSERT_ERROR(
	    (arrayLength >= MIN_SHORT_ARRAY_SIZE) && (arrayLength <= MAX_SHORT_ARRAY_SIZE));

    FCPPT_ASSERT_ERROR(
	    (batchSize * arrayLength) <= MAX_BATCH_ELEMENTS);

    FCPPT_ASSERT_ERROR(
	    (batchSize * arrayLength) % (4 * WORKGROUP_SIZE) == 0);

    return
    	scanExclusiveLocal1(
		d_Dst,
		d_Src,
		batchSize,
		arrayLength);
}

////////////////////////////////////////////////////////////////////////////////
// Large scan launcher
////////////////////////////////////////////////////////////////////////////////
void
flakelib::marching_cubes::scan::scanExclusiveLocal2(
    cl_mem d_Buffer,
    cl_mem d_Dst,
    cl_mem d_Src,
    cl_uint n,
    cl_uint size)
{
    cl_int ciErrNum;
    size_t localWorkSize, globalWorkSize;

    cl_uint elements = n * size;
    ciErrNum  = clSetKernelArg(ckScanExclusiveLocal2.impl(), 0, sizeof(cl_mem), (void *)&d_Buffer);
    ciErrNum |= clSetKernelArg(ckScanExclusiveLocal2.impl(), 1, sizeof(cl_mem), (void *)&d_Dst);
    ciErrNum |= clSetKernelArg(ckScanExclusiveLocal2.impl(), 2, sizeof(cl_mem), (void *)&d_Src);
    ciErrNum |= clSetKernelArg(ckScanExclusiveLocal2.impl(), 3, 2 * WORKGROUP_SIZE * sizeof(cl_uint), NULL);
    ciErrNum |= clSetKernelArg(ckScanExclusiveLocal2.impl(), 4, sizeof(cl_uint), (void *)&elements);
    ciErrNum |= clSetKernelArg(ckScanExclusiveLocal2.impl(), 5, sizeof(cl_uint), (void *)&size);
    FCPPT_ASSERT_ERROR(
	    ciErrNum == CL_SUCCESS);

    localWorkSize = WORKGROUP_SIZE;
    globalWorkSize = iSnapUp(elements, WORKGROUP_SIZE);

    ciErrNum = clEnqueueNDRangeKernel(command_queue_.impl(), ckScanExclusiveLocal2.impl(), 1, NULL, &globalWorkSize, &localWorkSize, 0, NULL, NULL);

    FCPPT_ASSERT_ERROR(
	    ciErrNum == CL_SUCCESS);
}

std::size_t
flakelib::marching_cubes::scan::uniformUpdate(
    cl_mem d_Dst,
    cl_mem d_Buffer,
    cl_uint n)
{
    cl_int ciErrNum;
    size_t localWorkSize, globalWorkSize;

    ciErrNum  = clSetKernelArg(ckUniformUpdate.impl(), 0, sizeof(cl_mem), (void *)&d_Dst);
    ciErrNum |= clSetKernelArg(ckUniformUpdate.impl(), 1, sizeof(cl_mem), (void *)&d_Buffer);
    FCPPT_ASSERT_ERROR(
	    ciErrNum == CL_SUCCESS);

    localWorkSize = WORKGROUP_SIZE;
    globalWorkSize = n * WORKGROUP_SIZE;

    ciErrNum = clEnqueueNDRangeKernel(command_queue_.impl(), ckUniformUpdate.impl(), 1, NULL, &globalWorkSize, &localWorkSize, 0, NULL, NULL);
    FCPPT_ASSERT_ERROR(
	    ciErrNum == CL_SUCCESS);

    return localWorkSize;
}

std::size_t
flakelib::marching_cubes::scan::scanExclusiveLarge(
    cl_mem d_Dst,
    cl_mem d_Src,
    cl_uint batchSize,
    cl_uint arrayLength)
{
    //Check power-of-two factorization
    cl_uint log2L;
    cl_uint factorizationRemainder = factorRadix2(log2L, arrayLength);
    FCPPT_ASSERT_ERROR(
	    factorizationRemainder == 1);

    //Check supported size range
    FCPPT_ASSERT_ERROR(
	    (arrayLength >= MIN_LARGE_ARRAY_SIZE) && (arrayLength <= MAX_LARGE_ARRAY_SIZE));

    //Check total batch size limit
    FCPPT_ASSERT_ERROR(
	    (batchSize * arrayLength) <= MAX_BATCH_ELEMENTS);

    scanExclusiveLocal1(
        d_Dst,
        d_Src,
        (batchSize * arrayLength) / (4 * WORKGROUP_SIZE),
        4 * WORKGROUP_SIZE);

    scanExclusiveLocal2(
        buffer_.impl(),
        d_Dst,
        d_Src,
        batchSize,
        arrayLength / (4 * WORKGROUP_SIZE));

    return
	    uniformUpdate(
		d_Dst,
		buffer_.impl(),
		(batchSize * arrayLength) / (4 * WORKGROUP_SIZE));
}
