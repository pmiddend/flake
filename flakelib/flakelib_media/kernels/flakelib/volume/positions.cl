#define FLAKE_VOLUME_FRONT_OF(w,pos)\
	(w) * (w) * min((w)-1,pos.z+1) + (w) * pos.y + pos.x

#define FLAKE_VOLUME_RIGHT_BOTTOM_OF(w,pos)\
	(w) * (w) * pos.z + (w) * min((w)-1,pos.y+1) + min((w)-1,pos.x + 1)
