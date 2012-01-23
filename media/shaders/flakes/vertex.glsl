#version 140

$$$HEADER$$$

void
main()
{
	float
		distance_camera_point =
			distance(
					position.xyz - camera_position),
		unnormalized_distance =
			1.0 -
			min(
				maximum_distance,
				distance_camera_point),
		normalized_distance =
			unnomalized_distance / maximum_distance,
		maxed_distance =
			max(
				normalized_distance,
				0.01);

	gl_PointSize =
		maxed_distance * point_size;

	gl_Position =
		mvp *
		vec4(position.xyz,1.0);
}
