// Planar positions

#define FLAKE_PLANAR_LEFT_OF(buffer_width,currentpos)\
	(buffer_width) * currentpos.y + max(0,currentpos.x-1)

#define FLAKE_PLANAR_LEFT_TOP_OF(buffer_width,currentpos)\
	(buffer_width) * max(0,currentpos.y-1) + max(0,currentpos.x-1)

#define FLAKE_PLANAR_RIGHT_OF(buffer_width,currentpos)\
	(buffer_width) * currentpos.y + min((buffer_width)-1,currentpos.x + 1)

#define FLAKE_PLANAR_RIGHT_TOP_OF(buffer_width,currentpos)\
	(buffer_width) * max(0,currentpos.y-1) + min((buffer_width)-1,currentpos.x + 1)

#define FLAKE_PLANAR_TOP_OF(buffer_width,currentpos)\
	(buffer_width) * max(0,currentpos.y-1) + currentpos.x

#define FLAKE_PLANAR_BOTTOM_OF(buffer_width,currentpos)\
	(buffer_width) * min((buffer_width)-1,currentpos.y+1) + currentpos.x

#define FLAKE_PLANAR_LEFT_BOTTOM_OF(buffer_width,currentpos)\
	(buffer_width) * min((buffer_width)-1,currentpos.y+1) + max(0,currentpos.x-1)

#define FLAKE_PLANAR_RIGHT_BOTTOM_OF(buffer_width,currentpos)\
	(buffer_width) * min((buffer_width)-1,currentpos.y+1) + min((buffer_width)-1,currentpos.x + 1)

#define FLAKE_PLANAR_AT(buffer_width,currentpos)\
	(buffer_width) * currentpos.y + currentpos.x

// Volume positions
#define FLAKE_VOLUME_AT(w,pos)\
	(w) * (w) * pos.z + (w) * pos.y + pos.x

#define FLAKE_VOLUME_LEFT_OF(w,pos)\
	(w) * (w) * pos.z + (w) * pos.y + max(0,pos.x-1)

#define FLAKE_VOLUME_RIGHT_OF(w,pos)\
	(w) * (w) * pos.z + (w) * pos.y + min((w)-1,pos.x + 1)

#define FLAKE_VOLUME_TOP_OF(w,pos)\
	(w) * (w) * pos.z + (w) * max(0,pos.y-1) + pos.x

#define FLAKE_VOLUME_BOTTOM_OF(w,pos)\
	(w) * (w) * pos.z + (w) * min((w)-1,pos.y+1) + pos.x

#define FLAKE_VOLUME_BACK_OF(w,pos)\
	(w) * (w) * max(0,pos.z-1) + (w) * pos.y + pos.x

#define FLAKE_VOLUME_FRONT_OF(w,pos)\
	(w) * (w) * min((w)-1,pos.z+1) + (w) * pos.y + pos.x
