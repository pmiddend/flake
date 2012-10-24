#include <flakelib/kernel_name.cl>
#include <flakelib/planar/global_size.cl>
#include <flakelib/planar/at.cl>
#include <flakelib/planar/left_of.cl>
#include <flakelib/planar/right_of.cl>
#include <flakelib/planar/top_of.cl>
#include <flakelib/planar/bottom_of.cl>
#include <flakelib/kernel_argument.cl>
#include <flakelib/planar/current_position.cl>

kernel void
FLAKELIB_KERNEL_NAME(vorticity_apply)(
	global float const *FLAKELIB_KERNEL_ARGUMENT(boundary),
	global float2 const *FLAKELIB_KERNEL_ARGUMENT(velocity),
	global float *FLAKELIB_KERNEL_ARGUMENT(output),
	uint const FLAKELIB_KERNEL_ARGUMENT(buffer_pitch))
{
	int2 const position =
		flakelib_planar_current_position();

	if(position.x <= 2 || position.y <= 2 || position.x >= buffer_pitch-3 || position.y >= buffer_pitch-3)
	{
		output[flakelib_planar_at(
					buffer_pitch,
					position)] =
			0.0f;
		return;
	}

	int const
		current_index =
			flakelib_planar_at(
				buffer_pitch,
				position),
		left_index =
			flakelib_planar_left_of(
				buffer_pitch,
				flakelib_planar_global_size(),
				position),
		right_index =
			flakelib_planar_right_of(
				buffer_pitch,
				flakelib_planar_global_size(),
				position),
		top_index =
			flakelib_planar_top_of(
				buffer_pitch,
				flakelib_planar_global_size(),
				position),
		bottom_index =
			flakelib_planar_bottom_of(
				buffer_pitch,
				flakelib_planar_global_size(),
				position);

	// The mix here hopefully enforces that the boundaries are ignored. The
	// velocity at the boundary is set to zero, which under normal
	// circumstances contributes to the vorticity.
	float2
		left =
			//(1.0f - boundary[left_index]) * velocity[left_index],
			//velocity[left_index],
			mix(
				velocity[left_index],
				velocity[right_index],
				boundary[left_index]),
		right =
			mix(
				velocity[right_index],
				velocity[left_index],
				boundary[right_index]),
			//(1.0f - boundary[right_index]) * velocity[right_index],
			//velocity[right_index],
		top =
			mix(
				velocity[top_index],
				velocity[bottom_index],
				boundary[top_index]),
			//(1.0f - boundary[top_index]) * velocity[top_index],
			//velocity[top_index],
		bottom =
			mix(
				velocity[bottom_index],
				velocity[top_index],
				boundary[bottom_index]);
			//(1.0f - boundary[bottom_index]) * velocity[bottom_index];
			// velocity[bottom_index];

	output[current_index] =
//		0.5f * ((right.y - left.y) - (top.x - bottom.x));
		0.5f * ((right.y - left.y) - (bottom.x - top.x));
}

kernel void
FLAKELIB_KERNEL_NAME(vorticity_gradient_and_cross)(
	float const FLAKELIB_KERNEL_ARGUMENT(vorticity_strength),
	uint const FLAKELIB_KERNEL_ARGUMENT(buffer_pitch),
	global float const *FLAKELIB_KERNEL_ARGUMENT(vorticity),
	global float2 const *FLAKELIB_KERNEL_ARGUMENT(velocity),
	global float2 *FLAKELIB_KERNEL_ARGUMENT(output),
	float const FLAKELIB_KERNEL_ARGUMENT(dt))
{
	int2 const position =
		flakelib_planar_current_position();

	/*
	if(position.x <= 2 || position.y <= 2 || position.x >= buffer_pitch-3 || position.y >= buffer_pitch-3)
	*/
	if(position.x == 0 || position.y == 0 || position.x == buffer_pitch-1 || position.y == buffer_pitch-1)
	{
		output[flakelib_planar_at(
					buffer_pitch,
					position)] =
			velocity[
				flakelib_planar_at(
					buffer_pitch,
					position)];
		return;
	}

	int const
		current_index =
			flakelib_planar_at(
				buffer_pitch,
				position),
		left_index =
			flakelib_planar_left_of(
				buffer_pitch,
				flakelib_planar_global_size(),
				position),
		right_index =
			flakelib_planar_right_of(
				buffer_pitch,
				flakelib_planar_global_size(),
				position),
		top_index =
			flakelib_planar_top_of(
				buffer_pitch,
				flakelib_planar_global_size(),
				position),
		bottom_index =
		flakelib_planar_bottom_of(
			buffer_pitch,
				flakelib_planar_global_size(),
				position);

	float const
		left =
			fabs(vorticity[left_index]),
		right =
			fabs(vorticity[right_index]),
		top =
			fabs(vorticity[top_index]),
		bottom =
			fabs(vorticity[bottom_index]);

	float2 const gradient =
		0.5f *
		(float2)(
			right-left,
			bottom-top);
//			top-bottom);

	float const gradient_magnitude =
		fast_length(gradient);

	// I _think_ this guard against small gradient magnitude makes sense.
	// Otherwise, we have a division by zero (or something like that) and have
	// a gradient which simply doesn't exist (it should be zero).
	float2 const normalized_gradient =
		gradient_magnitude < 0.01f
		?
			(float2)(0.0f)
		:
			gradient / gradient_magnitude;

	output[current_index] =
		velocity[current_index] +
		dt *
		min(
			1.0f,
			vorticity[current_index]) *
		vorticity_strength *
		min(
			(float2)(1.0f,1.0f),
			(float2)(normalized_gradient.y,-normalized_gradient.x));
}

kernel void
FLAKELIB_KERNEL_NAME(vorticity_confinement_data)(
	float const FLAKELIB_KERNEL_ARGUMENT(vorticity_strength),
	uint const FLAKELIB_KERNEL_ARGUMENT(buffer_pitch),
	global float const *FLAKELIB_KERNEL_ARGUMENT(vorticity),
	global float2 *FLAKELIB_KERNEL_ARGUMENT(output),
	float const FLAKELIB_KERNEL_ARGUMENT(dt))
{
	int2 const position =
		flakelib_planar_current_position();

	int const
		current_index =
			flakelib_planar_at(
				buffer_pitch,
				position),
		left_index =
			flakelib_planar_left_of(
				buffer_pitch,
				flakelib_planar_global_size(),
				position),
		right_index =
			flakelib_planar_right_of(
				buffer_pitch,
				flakelib_planar_global_size(),
				position),
		top_index =
			flakelib_planar_top_of(
				buffer_pitch,
				flakelib_planar_global_size(),
				position),
		bottom_index =
			flakelib_planar_bottom_of(
				buffer_pitch,
				flakelib_planar_global_size(),
				position);

	float const
		left =
			fabs(vorticity[left_index]),
		right =
			fabs(vorticity[right_index]),
		top =
			fabs(vorticity[top_index]),
		bottom =
			fabs(vorticity[bottom_index]);

	float2 const gradient =
		0.5f *
		(float2)(
			right-left,
			bottom-top);
//			top-bottom);

	float const gradient_magnitude =
		fast_length(gradient);

	// I _think_ this guard against small gradient magnitude makes sense.
	// Otherwise, we have a division by zero (or something like that) and have
	// a gradient which simply doesn't exist (it should be zero).
	float2 const normalized_gradient =
		gradient_magnitude < 0.01f
		?
			(float2)(0.0f)
		:
			gradient / gradient_magnitude;

	output[current_index] =
		//normalized_gradient;
		vorticity[current_index] *
		(float2)(normalized_gradient.y,-normalized_gradient.x);
		/*
		dt *
		vorticity[current_index] *
		vorticity_strength *
		normalized_gradient.yx*/;
}
