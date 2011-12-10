#ifndef FLAKE_FLOAT_HANDLING_CL_INCLUDED
#define FLAKE_FLOAT_HANDLING_CL_INCLUDED

#define FLAKE_READ_IMAGE_FUNCTION read_imagef
#define FLAKE_WRITE_IMAGE_FUNCTION write_imagef

typedef
float
flake_real;

typedef
float2
flake_real2;

typedef
float4
flake_real4;

#define FLAKE_REAL_LIT(x) x ## f

#define FLAKE_CONVERT_REAL2 convert_float2

#define FLAKE_FROM_FLOAT(x) x

#endif
