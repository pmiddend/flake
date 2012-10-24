#include <flakelib/kernel_name.cl>
#include <flakelib/kernel_argument.cl>

kernel void
FLAKELIB_KERNEL_NAME(fill_buffer_apply)(
	global float *FLAKELIB_KERNEL_ARGUMENT(input),
	float const FLAKELIB_KERNEL_ARGUMENT(value))
{
	input[get_global_id(0)] =
		value;
}
