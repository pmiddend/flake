kernel void fill_with_arrows(
	global float4 *buffer)
{
	buffer[get_global_id(0)] =
		(float4)(
			1.0f,
			0.0f,
			0.0f,
			1.0f);
}
