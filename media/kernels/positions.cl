#define FLAKE_LEFT_OF(buffer_width,currentpos) buffer_width * currentpos.y + max(0,currentpos.x-1)
#define FLAKE_RIGHT_OF(buffer_width,currentpos) buffer_width * currentpos.y + min(buffer_width,currentpos.x + 1)
#define FLAKE_TOP_OF(buffer_width,currentpos) buffer_width * max(0,currentpos.y-1) + currentpos.x
#define FLAKE_BOTTOM_OF(buffer_width,currentpos) buffer_width * min(buffer_width,currentpos.y+1) + currentpos.x
#define FLAKE_AT(buffer_width,currentpos) buffer_width * currentpos.y + currentpos.x
