#include <flakelib/kernel_name.cl>
#include <flakelib/kernel_argument.cl>


//Passed down with -D option on clBuildProgram
//Must be a power of two

// Naive inclusive scan: O(N * log2(N)) operations
// Allocate 2 * 'size' local memory, initialize the first half
// with 'size' zeros avoiding if(pos >= offset) condition evaluation
// and saving instructions
inline uint
scan1Inclusive(
	uint const idata,
	local uint *local_data,
	uint const local_data_size,
	global uint *debug_buffer,
	uint const size)
{
	uint pos =
		2u *
		get_local_id(0) -
		(get_local_id(0) & (size - 1u));

	if(pos >= local_data_size)
		debug_buffer[0] = 1;

	local_data[pos] =
		0;

	pos +=
		size;

	if(pos >= local_data_size)
		debug_buffer[0] = 1;

	local_data[pos] =
		idata;

	for(
		uint offset =
			1;
		offset < size;
		offset <<= 1)
	{
	    barrier(CLK_LOCAL_MEM_FENCE);

		if(pos >= local_data_size || pos < offset)
			debug_buffer[0] = 1;

	    uint const t =
	    	local_data[pos] + local_data[pos - offset];

	    barrier(CLK_LOCAL_MEM_FENCE);

		if(pos >= local_data_size)
			debug_buffer[0] = 1;

	    local_data[pos] = t;
	}

	if(pos >= local_data_size)
		debug_buffer[0] = 1;

	return
		local_data[pos];
}

inline uint
scan1Exclusive(
	uint idata,
	local uint *l_Data,
	uint const local_data_size,
	global uint *debug_buffer,
	uint size)
{
	return
		scan1Inclusive(
			idata,
			l_Data,
			local_data_size,
			debug_buffer,
			size) -
		idata;
}


// Vector scan: the array to be scanned is stored in work-item private memory
// as uint4
inline uint4
scan4Inclusive(
	uint4 data4,
	local uint *local_data,
	uint const local_data_size,
	global uint *debug_buffer,
	uint const size)
{
	// Prefix-sum the 4 vector components
	data4.y += data4.x;
	data4.z += data4.y;
	data4.w += data4.z;

	// Level-1 exclusive scan
	uint val =
		scan1Inclusive(
			data4.w,
			local_data,
			local_data_size,
			debug_buffer,
			size / 4u) -
		data4.w;

	return
		(data4 + (uint4)val);
}

inline uint4
scan4Exclusive(
	uint4 const data4,
	local uint *local_data,
	uint const local_data_size,
	global uint *debug_buffer,
	uint const size)
{
	return
		scan4Inclusive(
			data4,
			local_data,
			local_data_size,
			debug_buffer,
			size) -
		data4;
}

// This scans each 4-component vector in parallel
kernel
__attribute__((reqd_work_group_size(WORKGROUP_SIZE, 1, 1)))
void
FLAKELIB_KERNEL_NAME(exclusive_local1)(
    global uint4 *FLAKELIB_KERNEL_ARGUMENT(destination),
    uint const FLAKELIB_KERNEL_ARGUMENT(destination_size),
    global uint4 const *FLAKELIB_KERNEL_ARGUMENT(source),
    uint const FLAKELIB_KERNEL_ARGUMENT(source_size),
    local uint *FLAKELIB_KERNEL_ARGUMENT(local_data),
    uint const FLAKELIB_KERNEL_ARGUMENT(local_data_size),
    uint const FLAKELIB_KERNEL_ARGUMENT(size),
    global uint *FLAKELIB_KERNEL_ARGUMENT(debug_buffer))
{
	if(get_global_id(0) >= destination_size || get_global_id(0) >= source_size)
		debug_buffer[0] = 1;

	destination[get_global_id(0)] =
		scan4Exclusive(
			source[get_global_id(0)],
			local_data,
			local_data_size,
			debug_buffer,
			size);
}

// Exclusive scan of top elements of bottom-level scans (4 * THREADBLOCK_SIZE)
kernel __attribute__((reqd_work_group_size(WORKGROUP_SIZE, 1, 1)))
void
FLAKELIB_KERNEL_NAME(exclusive_local2)(
    global uint *FLAKELIB_KERNEL_ARGUMENT(d_Buf),
    uint const FLAKELIB_KERNEL_ARGUMENT(d_Buf_size),
    global uint *FLAKELIB_KERNEL_ARGUMENT(d_Dst),
    uint const FLAKELIB_KERNEL_ARGUMENT(d_Dst_size),
    global uint *FLAKELIB_KERNEL_ARGUMENT(d_Src),
    uint const FLAKELIB_KERNEL_ARGUMENT(d_Src_size),
    local uint *FLAKELIB_KERNEL_ARGUMENT(l_Data),
    uint const FLAKELIB_KERNEL_ARGUMENT(l_Data_size),
    uint const FLAKELIB_KERNEL_ARGUMENT(N),
    uint const FLAKELIB_KERNEL_ARGUMENT(arrayLength),
    global uint *FLAKELIB_KERNEL_ARGUMENT(debug_buffer))
{
    // Load top elements
    // Convert results of bottom-level scan back to inclusive
    // Skip loads and stores for inactive work-items of the work-group with highest index(pos >= N)
    uint data = 0;
    if(get_global_id(0) < N)
    {
	    size_t const index =
	    	(4 * WORKGROUP_SIZE - 1) + (4 * WORKGROUP_SIZE) * get_global_id(0);

	    if(index >= d_Dst_size || index >= d_Src_size)
		    debug_buffer[0] = 1;

	    data =
		d_Dst[index] +
		d_Src[index];
    }

    // Compute
    uint odata =
    	scan1Exclusive(
		data,
		l_Data,
		l_Data_size,
		debug_buffer,
		arrayLength);

    // Avoid out-of-bound access
    if(get_global_id(0) < N)
    {
	    if(get_global_id(0) >= d_Buf_size)
		    debug_buffer[0] = 1;
        d_Buf[get_global_id(0)] = odata;
    }
}

// Final step of large-array scan: combine basic inclusive scan with exclusive
// scan of top elements of input arrays
kernel __attribute__((reqd_work_group_size(WORKGROUP_SIZE, 1, 1)))
void
FLAKELIB_KERNEL_NAME(uniform_update)(
    global uint4 *FLAKELIB_KERNEL_ARGUMENT(d_Data),
    uint const FLAKELIB_KERNEL_ARGUMENT(d_Data_size),
    global uint *FLAKELIB_KERNEL_ARGUMENT(d_Buf),
    uint const FLAKELIB_KERNEL_ARGUMENT(d_Buf_size),
    global uint *FLAKELIB_KERNEL_ARGUMENT(debug_buffer))
{
	local uint buf[1];

	if(get_global_id(0) >= d_Data_size)
		debug_buffer[0] = 1;

	uint4 data4 =
		d_Data[get_global_id(0)];

	if(get_local_id(0) == 0)
	{
		if(get_group_id(0) >= d_Buf_size)
			debug_buffer[0] = 1;
		buf[0] = d_Buf[get_group_id(0)];
	}

	barrier(CLK_LOCAL_MEM_FENCE);
	data4 += (uint4)buf[0];
	d_Data[get_global_id(0)] =
		data4;
}
