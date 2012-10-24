#include <flakelib/kernel_name.cl>
#include <flakelib/kernel_argument.cl>


//Passed down with -D option on clBuildProgram
//Must be a power of two
#define WORKGROUP_SIZE 256

// Naive inclusive scan: O(N * log2(N)) operations
// Allocate 2 * 'size' local memory, initialize the first half
// with 'size' zeros avoiding if(pos >= offset) condition evaluation
// and saving instructions
inline uint
scan1Inclusive(
	uint const idata,
	local uint *local_data,
	uint const size)
{
	uint pos =
		2u *
		get_local_id(0) -
		(get_local_id(0) & (size - 1u));

	local_data[pos] =
		0;

	pos +=
		size;

	local_data[pos] =
		idata;

	for(
		uint offset =
			1;
		offset < size;
		offset <<= 1)
	{
	    barrier(CLK_LOCAL_MEM_FENCE);

	    uint const t =
	    	local_data[pos] + local_data[pos - offset];

	    barrier(CLK_LOCAL_MEM_FENCE);

	    local_data[pos] = t;
	}

	return
		local_data[pos];
}

inline uint scan1Exclusive(uint idata, __local uint *l_Data, uint size){
return scan1Inclusive(idata, l_Data, size) - idata;
}


// Vector scan: the array to be scanned is stored in work-item private memory
// as uint4
inline uint4
scan4Inclusive(
	uint4 data4,
	local uint *local_data,
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
			size / 4u) -
		data4.w;

	return
		(data4 + (uint4)val);
}

inline uint4
scan4Exclusive(
	uint4 const data4,
	local uint *local_data,
	uint const size)
{
	return
		scan4Inclusive(
			data4,
			local_data,
			size) -
		data4;
}

// This scans each 4-component vector in parallel
kernel
__attribute__((reqd_work_group_size(WORKGROUP_SIZE, 1, 1)))
void
FLAKELIB_KERNEL_NAME(scanExclusiveLocal1)(
    global uint4 *FLAKELIB_KERNEL_ARGUMENT(destination),
    global uint4 const *FLAKELIB_KERNEL_ARGUMENT(source),
    local uint *FLAKELIB_KERNEL_ARGUMENT(local_data),
    uint const FLAKELIB_KERNEL_ARGUMENT(size))
{
    destination[get_global_id(0)] =
    	scan4Exclusive(
		source[get_global_id(0)],
		local_data,
		size);
}

// Exclusive scan of top elements of bottom-level scans (4 * THREADBLOCK_SIZE)
kernel __attribute__((reqd_work_group_size(WORKGROUP_SIZE, 1, 1)))
void
FLAKELIB_KERNEL_NAME(scanExclusiveLocal2)(
    global uint *FLAKELIB_KERNEL_ARGUMENT(d_Buf),
    global uint *FLAKELIB_KERNEL_ARGUMENT(d_Dst),
    global uint *FLAKELIB_KERNEL_ARGUMENT(d_Src),
    local uint *FLAKELIB_KERNEL_ARGUMENT(l_Data),
    uint const FLAKELIB_KERNEL_ARGUMENT(N),
    uint const FLAKELIB_KERNEL_ARGUMENT(arrayLength))
{
    // Load top elements
    // Convert results of bottom-level scan back to inclusive
    // Skip loads and stores for inactive work-items of the work-group with highest index(pos >= N)
    uint data = 0;
    if(get_global_id(0) < N)
    data =
        d_Dst[(4 * WORKGROUP_SIZE - 1) + (4 * WORKGROUP_SIZE) * get_global_id(0)] +
        d_Src[(4 * WORKGROUP_SIZE - 1) + (4 * WORKGROUP_SIZE) * get_global_id(0)];

    // Compute
    uint odata = scan1Exclusive(data, l_Data, arrayLength);

    // Avoid out-of-bound access
    if(get_global_id(0) < N)
        d_Buf[get_global_id(0)] = odata;
}

// Final step of large-array scan: combine basic inclusive scan with exclusive
// scan of top elements of input arrays
kernel __attribute__((reqd_work_group_size(WORKGROUP_SIZE, 1, 1)))
void
FLAKELIB_KERNEL_NAME(uniformUpdate)(
    global uint4 *FLAKELIB_KERNEL_ARGUMENT(d_Data),
    global uint *FLAKELIB_KERNEL_ARGUMENT(d_Buf))
{
	local uint buf[1];

	uint4 data4 =
		d_Data[get_global_id(0)];

	if(get_local_id(0) == 0)
		buf[0] = d_Buf[get_group_id(0)];

	barrier(CLK_LOCAL_MEM_FENCE);
	data4 += (uint4)buf[0];
	d_Data[get_global_id(0)] =
		data4;
}
