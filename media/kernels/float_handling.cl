#ifndef FLAKE_FLOAT_HANDLING_CL_INCLUDED
#define FLAKE_FLOAT_HANDLING_CL_INCLUDED

#ifndef FLAKE_USE_HALF

#define FLAKE_READ_IMAGE_FUNCTION read_imagef
#define FLAKE_WRITE_IMAGE_FUNCTION write_imagef
#define FLAKE_REAL_LIT(x) x ## f
#define FLAKE_CONVERT_REAL2 convert_float2
#define FLAKE_FROM_FLOAT(x) x

typedef
float
flake_real;

typedef
float2
flake_real2;

typedef
float4
flake_real4;

#else

#define FLAKE_READ_IMAGE_FUNCTION read_imageh
#define FLAKE_WRITE_IMAGE_FUNCTION write_imageh
#define FLAKE_REAL_LIT(x) x ## h
#define FLAKE_CONVERT_REAL2 convert_half2
#define FLAKE_FROM_FLOAT(x) ((half)x)

typedef
half
flake_real;

typedef
half2
flake_real2;

typedef
half4
flake_real4;

#endif

#endif
