#include "ray.h"

internal ppm_data
read_world(const s8 *filename)
{
	puts("Reading files...");

	FILE *infile = fopen(filename, "r");

	if (!infile) {
		fprintf(stderr, "[ERROR] Unable to read input file %s.\n", filename);
		exit(1);
	}

	ppm_data world = {};
	s8 line[200] = {};

	while(fgets(line, sizeof(line), infile)) {
		if (!strcmp(line, "#Camera\n")) {
			fscanf(infile, "%f %f %f\n", &world.camera.position.X, &world.camera.position.Y, &world.camera.position.Z);
			fscanf(infile, "%f %f %f\n", &world.camera.gaze.X, &world.camera.gaze.Y, &world.camera.gaze.Z);
			fscanf(infile, "%f %f %f\n", &world.camera.up.X, &world.camera.up.Y, &world.camera.up.Z);
			fscanf(infile, "%f %f %f %f\n", &world.camera.film_left, &world.camera.film_right, &world.camera.film_bottom, &world.camera.film_top);
			fscanf(infile, "%f", &world.camera.film_dist);
			fscanf(infile, "%u %u\n", &world.camera.output_width, &world.camera.output_height);
		} else if (!strcmp(line, "#BackgroundColor\n")) {
			fscanf(infile, "%f %f %f\n", &world.background.R, &world.background.G, &world.background.B);
		} else if (!strcmp(line, "#AmbientLight\n")) {
			fscanf(infile, "%f %f %f\n", &world.ambient_light.R, &world.ambient_light.G, &world.ambient_light.B);
		} else if (!strcmp(line, "#MaxRecursionDepth\n")) {
			fscanf(infile, "%d\n", &world.max_recursion_depth);
		} else if (!strcmp(line, "#ShadowRayEpsilon\n")) {
			fscanf(infile, "%f\n", &world.shadow_ray_epsilon);
		} else if (!strcmp(line, "#PointLight\n")) {
			world.light_count++;
		} else if (!strcmp(line, "#Sphere\n")) {
			world.sphere_count++;
		} else if (!strcmp(line, "#Triangle\n")) {
			world.triangle_count++;
		} else if (!strcmp(line, "#Mesh\n")) {
			world.mesh_count++;
		} else if (!strcmp(line, "#Material\n")) {
			world.material_count++;
		} else if (!strcmp(line, "#VertexList\n")) {
			while (fgets(line, sizeof(line), infile)) {
				if (*line != '\n') {
					world.vertex_count++;
				} else {
					break;
				}
			}
		}
	}

	world.point_lights = (point_light *)malloc(world.light_count * sizeof(point_light));
	world.spheres = (sphere *)malloc(world.sphere_count * sizeof(sphere));
	world.triangles = (triangle *)malloc(world.triangle_count * sizeof(triangle));
	world.meshes = (mesh *)malloc(world.mesh_count * sizeof(mesh));
	world.materials = (material *)malloc(world.material_count * sizeof(material));
	world.vertex_list = (v3 *)malloc(world.vertex_count * sizeof(v3));

	rewind(infile);

	point_light *pl = world.point_lights;
	sphere *sp = world.spheres;
	triangle *tr = world.triangles;
	mesh *mh = world.meshes;
	material *mt = world.materials;
	v3 *vt = world.vertex_list;

	while(fgets(line, sizeof(line), infile)) {
		if (!strcmp(line, "#PointLight\n")) {
			fscanf(infile, "%d\n", &pl->id);
			fscanf(infile, "%f %f %f\n", &pl->position.X, &pl->position.Y, &pl->position.Z);
			fscanf(infile, "%f %f %f\n", &pl->intensity.R, &pl->intensity.G, &pl->intensity.B);
			pl++;
		} else if (!strcmp(line, "#Sphere\n")) {
			fscanf(infile, "%d\n", &sp->id);
			fscanf(infile, "%d\n", &sp->mat_id);
			fscanf(infile, "%d\n", &sp->center_id);
			fscanf(infile, "%f\n", &sp->radius);
			sp->center_id--;
			sp++;
		} else if (!strcmp(line, "#Triangle\n")) {
			fscanf(infile, "%d\n", &tr->id);
			fscanf(infile, "%d\n", &tr->mat_id);
			fscanf(infile, "%f %f %f\n", &tr->corner_ids[0], &tr->corner_ids[1], &tr->corner_ids[2]);
			tr->corner_ids[0]--;
			tr->corner_ids[1]--;
			tr->corner_ids[2]--;
			tr++;
		} else if (!strcmp(line, "#Mesh\n")) {
			fscanf(infile, "%d\n", &mh->id);
			fscanf(infile, "%d\n", &mh->mat_id);
			mh->tri_count = 0;
			while (fgets(line, sizeof(line), infile)) {
				if (*line != '\n') {
					mh->tri_count++;
				} else {
					break;
				}
			}
			mh++;
		} else if (!strcmp(line, "#Material\n")) {
			fscanf(infile, "%d\n", &mt->id);
			fscanf(infile, "%f %f %f\n", &mt->ambient.R, &mt->ambient.G, &mt->ambient.B);
			fscanf(infile, "%f %f %f\n", &mt->diffuse.R, &mt->diffuse.G, &mt->diffuse.B);
			fscanf(infile, "%f %f %f\n", &mt->specular.R, &mt->specular.G, &mt->specular.B);
			fscanf(infile, "%f\n", &mt->phong_exp);
			fscanf(infile, "%f %f %f\n", &mt->mirror.R, &mt->mirror.G, &mt->mirror.B);
			mt++;
		} else if (!strcmp(line, "#VertexList\n")) {
			while (fgets(line, sizeof(line), infile)) {
				if (*line != '\n') {
					sscanf(line, "%f %f %f\n", &vt->X, &vt->Y, &vt->Z);
				} else {
					break;
				}
				vt++;
			}
		}
	}

	for (u32 i = 0; i < world.mesh_count; ++i) {
		world.meshes[i].tri_ids = (v3 *)malloc(world.meshes[i].tri_count * sizeof(v3));
		world.meshes[i].normals = (v3 *)malloc(world.meshes[i].tri_count * sizeof(v3));
	}

	mesh *mhn = world.meshes;
	rewind(infile);

	while(fgets(line, sizeof(line), infile)) {
		if (!strcmp(line, "#Mesh\n")) {
			v3 *ti = mhn->tri_ids;
			v3 *ni = mhn->normals;
			fgets(line, sizeof(line), infile);
			fgets(line, sizeof(line), infile);

			while (fgets(line, sizeof(line), infile)) {
				if (*line != '\n') {
					sscanf(line, "%f %f %f\n", &ti->X, &ti->Y, &ti->Z);
					ti->X = ti->X - 1;
					ti->Y = ti->Y - 1;
					ti->Z = ti->Z - 1;
					v3 a = world.vertex_list[(u32)ti->X];
					v3 b = world.vertex_list[(u32)ti->Y];
					v3 c = world.vertex_list[(u32)ti->Z];
					*ni = normal_v3(cross(a - c, b - c));
					ti++;
					ni++;
				} else {
					break;
				}
			}
			mhn++;
		}
	}

	for (u32 i = 0; i < world.triangle_count; ++i) {
		triangle t = world.triangles[i];
		u32 a_id = t.corner_ids[0];
		u32 b_id = t.corner_ids[1];
		u32 c_id = t.corner_ids[2];
		v3 a = world.vertex_list[a_id];
		v3 b = world.vertex_list[b_id];
		v3 c = world.vertex_list[c_id];
		world.triangles[i].normal = normal_v3(cross(a - c, b - c));
	}

	fclose(infile);

	puts("Done...");

	return world;
}

internal u32
get_total_pixel_size(image_u32 image)
{
	return 3 * image.height * image.width;
}

internal image_u32
allocate_image(u32 width, u32 height)
{
	image_u32 image = {};
	image.magic_id[0] = 'P';
	image.magic_id[1] = '6';
	image.width = width;
	image.height = height;
	image.max_color_value = 255;

	u32 pixel_size = get_total_pixel_size(image);
	image.pixels = (u8 *)malloc(pixel_size);

	return image;
}

internal void
write_image(image_u32 image, const char *filename)
{
	FILE *outfile = fopen(filename, "wb");

	if (!outfile) {
		fprintf(stderr, "[ERROR] Unable to write output file %s.\n", filename);
		exit(1);
	}

	fprintf(outfile, "%s %d %d %d ", image.magic_id, image.width, image.height, image.max_color_value);
	fwrite(image.pixels, get_total_pixel_size(image), 1, outfile);

	fclose(outfile);
}

internal b32
is_inside_triangle(v3 intersect, v3 tri_a, v3 tri_b, v3 tri_c)
{
	b32 same_side_a = (dot_v3(cross(tri_c - tri_b, intersect - tri_b), cross(tri_c - tri_b, tri_a - tri_b)) >= 0) ? 1 : 0;
	b32 same_side_b = (dot_v3(cross(tri_c - tri_a, intersect - tri_a), cross(tri_c - tri_a, tri_b - tri_a)) >= 0) ? 1 : 0;
	b32 same_side_c = (dot_v3(cross(tri_a - tri_b, intersect - tri_b), cross(tri_a - tri_b, tri_c - tri_b)) >= 0) ? 1 : 0;

	if (same_side_a && same_side_b && same_side_c)
		return 1;

	return 0;
}

internal v3
ray_cast(ppm_data *world, v3 origin, v3 dest)
{
	material *materials = world->materials;
	point_light *lights = world->point_lights;
	v3 *vertex_list = world->vertex_list;
	f32 eps = world->shadow_ray_epsilon;

	v3 result = {};
	v3 next_origin = {};
	v3 next_normal = {};
	f32 tolerance = 0.0001f;
	f32 min_hit_dist = 0.001f;
	v3 attenuation = V3(1, 1, 1);
	f32 hit_dist = F32MAX;
	u32 hit_mat_id = 0;
	u32 hit_mat_index = 0;
	b32 hit_status = false;

	for (u32 k = 0; k < world->max_recursion_depth; ++k) {
		for (u32 i = 0; i < world->sphere_count; ++i) {
			sphere sphere = world->spheres[i];
			u32 center_id = sphere.center_id;
			v3 pos = vertex_list[center_id];
			f32 r = sphere.radius;

			f32 a = dot_v3(dest, dest);
			f32 b = 2.0f * dot_v3(dest, origin - pos);
			f32 c = dot_v3(origin - pos, origin - pos) - r * r;
			f32 D = b * b - 4.0f * a * c;
			f32 denom = 2.0f * a;

			if (D > tolerance){
				f32 tp = (- b + sqrtf(D)) / denom;
				f32 tn = (- b - sqrtf(D)) / denom;
				f32 t = tp;

				if ((tn > 0) && (tn < tp))
					t = tn;

				if ((t > min_hit_dist) && (t < hit_dist)){
					hit_dist = t;
					hit_mat_id = sphere.mat_id;
					hit_status = true;
					next_origin = origin + t * dest;
					next_normal = normal_v3(next_origin - pos);
				}
			}
		}

		for (u32 i = 0; i < world->triangle_count; ++i) {
			triangle triangle = world->triangles[i];
			v3 tri_index = triangle.corner_ids;
			v3 tri_a = vertex_list[(u32)tri_index[0]];
			v3 tri_b = vertex_list[(u32)tri_index[1]];
			v3 tri_c = vertex_list[(u32)tri_index[2]];
			v3 tri_cen = (tri_a + tri_b + tri_c) / 3;
			v3 tri_normal = triangle.normal;

			f32 denom = dot_v3(dest, tri_normal);

			if ((denom < -tolerance) || (denom > tolerance)) {
				f32 t = dot_v3(tri_cen - origin, tri_normal) / denom;
				v3 intersect = origin + t * dest;

				if (is_inside_triangle(intersect, tri_a, tri_b, tri_c) && (t > min_hit_dist) && (t < hit_dist)) {
					hit_dist = t;
					hit_mat_id = triangle.mat_id;
					hit_status = true;
					next_origin = origin + t * dest;
					next_normal = tri_normal;
				}
			}
		}

		for (u32 i = 0; i < world->mesh_count; ++i) {
			mesh mesh = world->meshes[i];

			for (u32 j = 0; j < mesh.tri_count; ++j) {
				v3 tri_index = mesh.tri_ids[j];
				v3 tri_a = vertex_list[(u32)tri_index[0]];
				v3 tri_b = vertex_list[(u32)tri_index[1]];
				v3 tri_c = vertex_list[(u32)tri_index[2]];
				v3 tri_cen = (tri_a + tri_b + tri_c) / 3.0f;
				v3 tri_normal = mesh.normals[j];

				f32 denom = dot_v3(dest, tri_normal);

				if ((denom < -tolerance) || (denom > tolerance)) {
					f32 t = dot_v3(tri_cen - origin, tri_normal) / denom;
					v3 intersect = origin + t * dest;

					if (is_inside_triangle(intersect, tri_a, tri_b, tri_c) && (t > min_hit_dist) && (t < hit_dist)) {
						hit_dist = t;
						hit_mat_id = mesh.mat_id;
						hit_status = true;
						next_origin = origin + t * dest;
						next_normal = tri_normal;
					}
				}
			}
		}

		v3 color = {};

		if (hit_status) {
			for (u32 i = 0; i < world->material_count; ++i)
				if (hit_mat_id == materials[i].id)
					hit_mat_index = i;

			material mat = materials[hit_mat_index];
			color = world->ambient_light * mat.ambient;

			for (u32 i = 0; i < world->light_count; ++i) {
				f32 light_dist = len_v3(lights[i].position - next_origin);
				v3 light_direction = normal_v3(lights[i].position - next_origin);
				f32 next_hit_dist = light_dist;
				b32 next_hit_status = false;
				v3 shadow_origin = next_origin + light_direction * eps;

				for (u32 i = 0; i < world->sphere_count; ++i){
					sphere sphere = world->spheres[i];
					u32 center_id = sphere.center_id;
					v3 pos = vertex_list[center_id];
					f32 r = sphere.radius;

					f32 a = dot_v3(light_direction, light_direction);
					f32 b = 2.0f * dot_v3(light_direction, shadow_origin - pos);
					f32 c = dot_v3(shadow_origin - pos, shadow_origin - pos) - r * r;
					f32 D = b * b - 4.0f * a * c;
					f32 denom = 2.0f * a;

					if (D > tolerance){
						f32 tp = (- b + sqrtf(D)) / denom;
						f32 tn = (- b - sqrtf(D)) / denom;
						f32 t = tp;

						if ((tn > 0) && (tn < tp))
							t = tn;

						if ((t > min_hit_dist) && (t < next_hit_dist))
							next_hit_status = true;
					}

				}

				if (next_hit_status)
					continue;

				for (u32 i = 0; i < world->triangle_count; ++i) {
					triangle triangle = world->triangles[i];
					v3 tri_index = triangle.corner_ids;
					v3 tri_a = vertex_list[(u32)tri_index[0]];
					v3 tri_b = vertex_list[(u32)tri_index[1]];
					v3 tri_c = vertex_list[(u32)tri_index[2]];
					v3 tri_cen = (tri_a + tri_b + tri_c) / 3.0f;
					v3 tri_normal = triangle.normal;

					f32 denom = dot_v3(light_direction, tri_normal);

					if ((denom < -tolerance) || (denom > tolerance)) {
						f32 t = dot_v3(tri_cen - shadow_origin, tri_normal)
							/ denom;
						v3 intersect = shadow_origin + t * light_direction;

						if (is_inside_triangle(intersect, tri_a, tri_b, tri_c) && (t > min_hit_dist) && (t < next_hit_dist)) {
							next_hit_status = true;
						}
					}
				}

				if (next_hit_status)
					continue;

				for (u32 i = 0; i < world->mesh_count; ++i) {
					mesh mesh = world->meshes[i];

					for (u32 j = 0; j < mesh.tri_count; ++j) {
						v3 tri_index = mesh.tri_ids[j];
						v3 tri_a = vertex_list[(u32)tri_index[0]];
						v3 tri_b = vertex_list[(u32)tri_index[1]];
						v3 tri_c = vertex_list[(u32)tri_index[2]];
						v3 tri_cen = (tri_a + tri_b + tri_c) / 3.0f;
						v3 tri_normal = mesh.normals[j];

						f32 denom = dot_v3(light_direction, tri_normal);

						if ((denom < -tolerance) || (denom > tolerance)) {
							f32 t = dot_v3(tri_cen - shadow_origin, tri_normal)
								/ denom;
							v3 intersect = shadow_origin + t * light_direction;

							if (is_inside_triangle(intersect, tri_a, tri_b, tri_c) && (t > min_hit_dist) && (t < next_hit_dist)) {
								next_hit_status = true;
							}
						}
					}
				}

				if (next_hit_status)
					continue;

				f32 cos_value = dot_v3(next_normal, light_direction);
				if (cos_value < 0)
					cos_value = 0;
				v3 intensity = lights[i].intensity / (light_dist * light_dist);
				color += mat.diffuse * cos_value * intensity;

				cos_value = 1;
				for (u32 j = 0; j < mat.phong_exp; ++j)
					cos_value *= dot_v3(next_normal, normal_v3(- dest + light_direction));
				if (cos_value < 0)
					cos_value = 0;
				color += mat.specular * cos_value * intensity;
			}

			if (color.R > 255)
				color.R = 255;
			if (color.G > 255)
				color.G = 255;
			if (color.B > 255)
				color.B = 255;

			result += attenuation * color;
			attenuation *= mat.mirror;

			origin = next_origin;

			v3 pure_bounce = dest - 2.0f*dot_v3(dest, next_normal)*next_normal;
			dest = normal_v3(pure_bounce);
		} else {
			result += attenuation * color;
			break;
		}
	}

	return result;
}

int
main(int arg_count, char **args)
{
	if (arg_count == 1){
		fprintf(stderr, "You need to enter a parameter\n");
		exit(1);
	} else if (arg_count != 2){
		fprintf(stderr, "Only 1 file is accepted as a parameter\n");
		exit(1);
	}

	ppm_data world = read_world(args[1]);

	image_u32 image = allocate_image(world.camera.output_width, world.camera.output_height);

	v3 camera_pos = world.camera.position;
	v3 camera_gaze = world.camera.gaze;
	v3 camera_up = world.camera.up;
	v3 camera_z = normal_v3(-camera_gaze);
	v3 camera_y = normal_v3(camera_up);
	v3 camera_x = normal_v3(cross(camera_z, camera_y));

	f32 film_left = world.camera.film_left;
	f32 film_right = world.camera.film_right;
	f32 film_bottom = world.camera.film_bottom;
	f32 film_top = world.camera.film_top;
	f32 film_dist = world.camera.film_dist;
	v3 film_center = camera_pos - film_dist * camera_z;
	f32 film_width = film_right - film_left;
	f32 film_height = film_top - film_bottom;
	f32 film_half_width = 0.5f * film_width;
	f32 film_half_height = 0.5f * film_height;

	u32 rays_per_pixel = 1;
	u32 pixel_size = get_total_pixel_size(image);
	u8 *out = &image.pixels[pixel_size - 1];

	for (u32 y = 0; y < image.height; ++y) {
		f32 film_y = film_bottom + film_height * ((f32)y / (f32)image.height);

		for (u32 x = 0; x < image.width; ++x) {
			f32 film_x = film_left + film_width * ((f32)x / (f32)image.width);
			v3 color = {};
			f32 contrib = 1.0f / (f32)rays_per_pixel;

			for (u32 i = 0; i < rays_per_pixel; ++i) {
				v3 film_pos = film_center + film_x*film_half_width*camera_x + film_y*film_half_height*camera_y;
				v3 ray_origin = camera_pos;
				v3 ray_destination = normal_v3(film_pos - camera_pos);
				color += contrib * ray_cast(&world, ray_origin, ray_destination);
			}

			*out-- = (u8)color.B;
			*out-- = (u8)color.G;
			*out-- = (u8)color.R;
		}

		if (!(y % 64)) {
			printf("\rRaytracing... %d%%", 100 * y / image.height);
			fflush(stdout);
		}
	}

	write_image(image, "output.ppm");

	printf("\nDone.\n");

	return 0;
}
