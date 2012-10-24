#ifndef FLAKELIB_RANDOM_FLOAT_VALUE_CL_INCLUDED
#define FLAKELIB_RANDOM_FLOAT_VALUE_CL_INCLUDED

float
flakelib_random_float_value(
	float const f)
{
	return (as_float(as_int(f) & 0x007fffff | 0x40000000) - 3.0f + 1.0f)/2.0f;
}

#endif
