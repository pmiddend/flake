#include <flakelib/kernel_name.cl>
#include <flakelib/kernel_argument.cl>

kernel void
FLAKELIB_KERNEL_NAME(mix_buffers_apply_add)(
	global float const *FLAKELIB_KERNEL_ARGUMENT(from),
	global float *FLAKELIB_KERNEL_ARGUMENT(to))
{
	to[get_global_id(0)] =
		from[get_global_id(0)]+to[get_global_id(0)];
}
