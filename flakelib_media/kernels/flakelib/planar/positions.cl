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


int2
flake_planar_global_size()
{
	return
		(int2)(
			get_global_id(0),
			get_global_id(1));
}
