#include <flakelib/volume/right_neighbors/load.cl>
#include <flakelib/volume/right_neighbors/float4.cl>
#include <flakelib/volume/right_neighbors/float.cl>
#include <flakelib/kernel_name.cl>
#include <flakelib/kernel_argument.cl>

// volume data
sampler_t volumeSampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
sampler_t tableSampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;


// compute position in 3d grid from 1d index
// only works for power of 2 sizes
int4
calcGridPos(
	uint i,
	uint4 gridSizeShift,
	uint4 gridSizeMask)
{
    int4 gridPos;
    gridPos.x = (i & gridSizeMask.x);
    gridPos.y = ((i >> gridSizeShift.y) & gridSizeMask.y);
    gridPos.z = ((i >> gridSizeShift.z) & gridSizeMask.z);
    return gridPos;
}

// classify voxel based on number of vertices it will generate
// one thread per voxel
kernel
void
FLAKELIB_KERNEL_NAME(classify_voxels)(
	global uint *FLAKELIB_KERNEL_ARGUMENT(debug_buffer),
	global uint *FLAKELIB_KERNEL_ARGUMENT(vertices_for_voxel),
	uint const FLAKELIB_KERNEL_ARGUMENT(vertices_for_voxel_size),
	global uint *FLAKELIB_KERNEL_ARGUMENT(voxel_occupation),
	uint const FLAKELIB_KERNEL_ARGUMENT(voxel_occupation_size),
	global float const *FLAKELIB_KERNEL_ARGUMENT(density),
	uint4 const FLAKELIB_KERNEL_ARGUMENT(grid_size),
	uint4 const FLAKELIB_KERNEL_ARGUMENT(grid_size_shift),
	uint4 const FLAKELIB_KERNEL_ARGUMENT(grid_size_mask),
	float const FLAKELIB_KERNEL_ARGUMENT(iso_value),
	read_only image2d_t FLAKELIB_KERNEL_ARGUMENT(numVertsTex))
{
	size_t const i =
		get_global_id(
			0u);

	int4 const gridPos =
		calcGridPos(
			i,
			grid_size_shift,
			grid_size_mask);

	flakelib_volume_right_neighbors_float right_neighbors;
	FLAKELIB_VOLUME_RIGHT_NEIGHBORS_LOAD(
	    density,
	    right_neighbors,
	    grid_size.x,
	    gridPos,
	    grid_size);

	// read field values at neighbouring grid vertices
	float field[8];

	field[0] = right_neighbors.at;
	field[1] = right_neighbors.right;
	field[2] = right_neighbors.rightbottom;
	field[3] = right_neighbors.bottom;
	field[4] = right_neighbors.back;
	field[5] = right_neighbors.backright;
	field[6] = right_neighbors.backrightbottom;
	field[7] = right_neighbors.backbottom;

	// calculate flag indicating if each vertex is inside or outside isosurface
	int cubeindex;
	cubeindex =  (field[0] < iso_value);
	cubeindex += (field[1] < iso_value)*2;
	cubeindex += (field[2] < iso_value)*4;
	cubeindex += (field[3] < iso_value)*8;
	cubeindex += (field[4] < iso_value)*16;
	cubeindex += (field[5] < iso_value)*32;
	cubeindex += (field[6] < iso_value)*64;
	cubeindex += (field[7] < iso_value)*128;

	// read number of vertices from texture
	uint const numVerts =
		read_imageui(
			numVertsTex,
			tableSampler,
			(int2)(cubeindex,0)).x;

#ifdef FLAKELIB_MARCHING_CUBES_DEBUG
	if(i >= vertices_for_voxel_size || i >= voxel_occupation_size)
		debug_buffer[0] = 1;
#endif

	vertices_for_voxel[i] = numVerts;
	voxel_occupation[i] = (numVerts > 0);
}

// compact voxel array
kernel
void
FLAKELIB_KERNEL_NAME(compact_voxels)(
	global uint *FLAKELIB_KERNEL_ARGUMENT(debug_buffer),
	global uint *FLAKELIB_KERNEL_ARGUMENT(compacted_voxel_occupation),
	uint const FLAKELIB_KERNEL_ARGUMENT(compacted_voxel_occupation_size),
	global uint const *FLAKELIB_KERNEL_ARGUMENT(voxel_occupation),
	uint const FLAKELIB_KERNEL_ARGUMENT(voxel_occupation_size),
	global uint const *FLAKELIB_KERNEL_ARGUMENT(summed_voxel_occupation),
	uint const FLAKELIB_KERNEL_ARGUMENT(summed_voxel_occupation_size))
{
	size_t const id =
		get_global_id(0);

#ifdef FLAKELIB_MARCHING_CUBES_DEBUG
	if(id >= voxel_occupation_size)
		debug_buffer[0] = 1;
#endif

	if (voxel_occupation[id])
	{
#ifdef FLAKELIB_MARCHING_CUBES_DEBUG
		if(id >= summed_voxel_occupation_size)
			debug_buffer[0] = 1;

		if(summed_voxel_occupation[id] >= compacted_voxel_occupation_size)
			debug_buffer[0] = 1;
#endif

		compacted_voxel_occupation[summed_voxel_occupation[id]] =
			(uint)id;
	}
}



/*
// compute interpolated vertex along an edge
float4 vertexInterp(float isolevel, float4 p0, float4 p1, float f0, float f1)
{
	if(fabs(f1 - f0) < 0.0001f)
		return p0;

	float const t =
		(isolevel - f0) /
		(f1 - f0);

	return
		mix(
			p0,
			p1,
			t);
}

// compute interpolated vertex position and normal along an edge
void vertexInterp2(float isolevel, float4 p0, float4 p1, float4 f0, float4 f1, float4* p, float4* n)
{
	float t = (isolevel - f0.w) / (f1.w - f0.w);
	*p = mix(p0, p1, t);
	(*n).x = mix(f0.x, f1.x, t);
	(*n).y = mix(f0.y, f1.y, t);
	(*n).z = mix(f0.z, f1.z, t);
//	n = normalize(n);
}
*/
void
vertexAndNormalInterp(
	float const isolevel,
	float4 const p0,
	float4 const p1,
	float const f0,
	float f1,
	float4 n0,
	float4 n1,
	local float4 *p,
	local float4 *n)
{
	if(fabs(f1 - f0) < 0.0001f)
	{
		*p = p0;
		*n = n0;
	}

	float const t =
		(isolevel - f0) /
		(f1 - f0);

	*p =
		mix(
			p0,
			p1,
			t);

	*n =
		mix(
			n0,
			n1,
			t);
}


/*
// calculate triangle normal
float4 calcNormal(float4 v0, float4 v1, float4 v2)
{
    float4 edge0 = v1 - v0;
    float4 edge1 = v2 - v0;
    // note - it's faster to perform normalization in vertex shader rather than here
    return cross(edge0, edge1);
}
*/

constant unsigned const indices[12][2] =
{
	{0u,1u},
	{1u,2u},
	{2u,3u},
	{3u,0u},
	{4u,5u},
	{5u,6u},
	{6u,7u},
	{7u,4u},
	{0u,4u},
	{1u,5u},
	{2u,6u},
	{3u,7u}
};


// version that calculates flat surface normal for each triangle
kernel
void
FLAKELIB_KERNEL_NAME(generate_triangles)(
	global uint *FLAKELIB_KERNEL_ARGUMENT(debug_buffer),
	global float4 *FLAKELIB_KERNEL_ARGUMENT(positions),
	uint const FLAKELIB_KERNEL_ARGUMENT(positions_size),
	global float4 *FLAKELIB_KERNEL_ARGUMENT(normals),
	uint const FLAKELIB_KERNEL_ARGUMENT(normals_size),
	global uint const *FLAKELIB_KERNEL_ARGUMENT(compacted_voxel_occupation),
	uint const FLAKELIB_KERNEL_ARGUMENT(compacted_voxel_occupation_size),
	global uint const *FLAKELIB_KERNEL_ARGUMENT(summed_vertices_for_voxel),
	uint const FLAKELIB_KERNEL_ARGUMENT(summed_vertices_for_voxel_size),
	global float const *FLAKELIB_KERNEL_ARGUMENT(volume),
	uint4 const FLAKELIB_KERNEL_ARGUMENT(grid_size),
	uint4 const FLAKELIB_KERNEL_ARGUMENT(grid_size_shift),
	uint4 const FLAKELIB_KERNEL_ARGUMENT(grid_size_mask),
	float const FLAKELIB_KERNEL_ARGUMENT(iso_value),
	uint const FLAKELIB_KERNEL_ARGUMENT(active_voxels),
	uint const FLAKELIB_KERNEL_ARGUMENT(max_verts),
	read_only image2d_t FLAKELIB_KERNEL_ARGUMENT(numVertsTex),
	read_only image2d_t FLAKELIB_KERNEL_ARGUMENT(triTex),
	global float4 const *FLAKELIB_KERNEL_ARGUMENT(gradient))
{
	uint const i =
		min(
			active_voxels - 1,
			get_global_id(0));

	uint const tid =
		get_local_id(
			0);

#ifdef FLAKELIB_MARCHING_CUBES_DEBUG
	if(i >= compacted_voxel_occupation_size)
		debug_buffer[0] = 1;
#endif

	uint const voxel =
		compacted_voxel_occupation[i];

	// compute position in 3d grid
	int4 const gridPos =
		calcGridPos(
			voxel,
			grid_size_shift,
			grid_size_mask);

	float4 const
		voxelSize =
			(float4)(
				1.0f,
				1.0f,
				1.0f,
				0.0f),
		p =
			(float4)(
				(float)gridPos.x,
				(float)gridPos.y,
				(float)gridPos.z,
				1.0f);

	// calculate cell vertex positions
	float4 v[8];
	v[0] = p;
	v[1] = p + (float4)(voxelSize.x, 0, 0,0);
	v[2] = p + (float4)(voxelSize.x, voxelSize.y, 0,0);
	v[3] = p + (float4)(0, voxelSize.y, 0,0);
	v[4] = p + (float4)(0, 0, voxelSize.z,0);
	v[5] = p + (float4)(voxelSize.x, 0, voxelSize.z,0);
	v[6] = p + (float4)(voxelSize.x, voxelSize.y, voxelSize.z,0);
	v[7] = p + (float4)(0, voxelSize.y, voxelSize.z,0);

	flakelib_volume_right_neighbors_float right_neighbors;
	FLAKELIB_VOLUME_RIGHT_NEIGHBORS_LOAD(
		volume,
		right_neighbors,
		grid_size.x,
		gridPos,
		grid_size);

	// read field values at neighbouring grid vertices
	float const field[8] =
	{
		right_neighbors.at,
		right_neighbors.right,
		right_neighbors.rightbottom,
		right_neighbors.bottom,
		right_neighbors.back,
		right_neighbors.backright,
		right_neighbors.backrightbottom,
		right_neighbors.backbottom
	};

	// Gradient begin
	flakelib_volume_right_neighbors_float4 right_gradient_neighbors;
	FLAKELIB_VOLUME_RIGHT_NEIGHBORS_LOAD(
	    gradient,
	    right_gradient_neighbors,
	    grid_size.x,
	    gridPos,
	    grid_size);

	float4 const gradient_field[8] =
	{
		right_gradient_neighbors.at,
		right_gradient_neighbors.right,
		right_gradient_neighbors.rightbottom,
		right_gradient_neighbors.bottom,
		right_gradient_neighbors.back,
		right_gradient_neighbors.backright,
		right_gradient_neighbors.backrightbottom,
		right_gradient_neighbors.backbottom
	};

	// Gradient end

	// Calculate which combination of "vertices are below/upwards isoLevel" we have
	int cubeindex;
	cubeindex =  (field[0] < iso_value);
	cubeindex += (field[1] < iso_value)*2;
	cubeindex += (field[2] < iso_value)*4;
	cubeindex += (field[3] < iso_value)*8;
	cubeindex += (field[4] < iso_value)*16;
	cubeindex += (field[5] < iso_value)*32;
	cubeindex += (field[6] < iso_value)*64;
	cubeindex += (field[7] < iso_value)*128;

	// find the vertices where the surface intersects the cube
	local float4 vertlist[16*NTHREADS];
	local float4 normlist[16*NTHREADS];

	// Put vertex and normal values of the current cube to local memory
	for(unsigned i = 0; i < 12; ++i)
	{
		unsigned const
			begin =
				indices[i][0],
			end =
				indices[i][1];

		unsigned const other_index =
			(i * NTHREADS) + tid;

#ifdef FLAKELIB_MARCHING_CUBES_DEBUG
		if(other_index >= 16*NTHREADS)
			debug_buffer[0] = 1;
#endif

		vertexAndNormalInterp(
			iso_value,
			// Cube vertex positions in global coordinates
			v[begin],
			v[end],
			// Density values at the cube vertices
			field[begin],
			field[end],
			// Gradient values at the cube vertices
			gradient_field[begin],
			gradient_field[end],
			&vertlist[other_index],
			&normlist[other_index]);
	}

	barrier(CLK_LOCAL_MEM_FENCE);

	// How many vertices do we have for this cube...
	uint const numVerts =
		read_imageui(
			numVertsTex,
			tableSampler,
			(int2)(cubeindex,0)).x;

	// Create numVerts/3 triangles...
	for(
		uint i = 0u;
		i < numVerts;
		i += 3u)
	{
#ifdef FLAKELIB_MARCHING_CUBES_DEBUG
		if(voxel >= summed_vertices_for_voxel_size)
			debug_buffer[0] = 1;
#endif

		uint const index =
			summed_vertices_for_voxel[voxel] + i;

		float4 v[3];
		float4 n[3];

		uint edge;
		edge = read_imageui(triTex, tableSampler, (int2)(i,cubeindex)).x;

#ifdef FLAKELIB_MARCHING_CUBES_DEBUG
		if((edge*NTHREADS)+tid >= 16*NTHREADS)
			debug_buffer[0] = 1;
#endif

		v[0] = vertlist[(edge*NTHREADS)+tid];
		n[0] = normlist[(edge*NTHREADS)+tid];

		edge = read_imageui(triTex, tableSampler, (int2)(i+1,cubeindex)).x;

#ifdef FLAKELIB_MARCHING_CUBES_DEBUG
		if((edge*NTHREADS)+tid >= 16*NTHREADS)
			debug_buffer[0] = 1;
#endif

		v[1] = vertlist[(edge*NTHREADS)+tid];
		n[1] = normlist[(edge*NTHREADS)+tid];

		edge = read_imageui(triTex, tableSampler, (int2)(i+2,cubeindex)).x;

#ifdef FLAKELIB_MARCHING_CUBES_DEBUG
		if((edge*NTHREADS)+tid >= 16*NTHREADS)
			debug_buffer[0] = 1;
#endif

		v[2] = vertlist[(edge*NTHREADS)+tid];
		n[2] = normlist[(edge*NTHREADS)+tid];

		// calculate triangle surface normal
		//float4 n = calcNormal(v[0], v[1], v[2]);

		if (index < (max_verts - 3))
		{
#ifdef FLAKELIB_MARCHING_CUBES_DEBUG
			if(index >= positions_size || index >= normals_size)
				debug_buffer[0] = 1;
			if(index+1 >= positions_size || index+1 >= normals_size)
				debug_buffer[0] = 1;
			if(index+2 >= positions_size || index+2 >= normals_size)
				debug_buffer[0] = 1;
#endif
			positions[index] = v[0];
			normals[index] = n[0];

			positions[index+1] = v[1];
			normals[index+1] = n[1];

			positions[index+2] = v[2];
			normals[index+2] = n[2];
		}
	}
}
