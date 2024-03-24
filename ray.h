#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define internal static
#define global static
#define local_persist static

#include "ray_math.h"

struct camera {
	v3 position;
	v3 gaze;
	v3 up;
	f32 film_left;
	f32 film_right;
	f32 film_bottom;
	f32 film_top;
	f32 film_dist;
	u32 output_width;
	u32 output_height;
};

struct material {
	u32 id;
	v3 ambient;
	v3 diffuse;
	v3 specular;
	v3 mirror;
	f32 phong_exp;
};

struct point_light {
	u32 id;
	v3 position;
	v3 intensity;
};

struct sphere {
	u32 id;
	u32 mat_id;
	u32 center_id;
	f32 radius;
};

struct triangle {
	u32 id;
	u32 mat_id;
	v3 corner_ids;
	v3 normal;
};

struct mesh {
	u32 id;
	u32 tri_count;
	u32 mat_id;
	v3 *tri_ids;
	v3 *normals;
};

struct ppm_data {
	u32 sphere_count;
	sphere *spheres;
	u32 triangle_count;
	triangle *triangles;
	u32 mesh_count;
	mesh *meshes;
	u32 material_count;
	material *materials;
	u32 light_count;
	point_light *point_lights;
	u32 vertex_count;
	v3 *vertex_list;
	camera camera;
	v3 background;
	v3 ambient_light;
	u32 max_recursion_depth;
	f32 shadow_ray_epsilon;
};

struct image_u32 {
	u8 magic_id[2];
	u32 width;
	u32 height;
	u32 max_color_value;
	u8 *pixels;
};
