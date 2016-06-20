#include "../include/engine.h"
#include "../include/utils.h"
#include "../include/mesh.h"
#include "../include/objLoader.h"
#include "SDL2/SDL_image.h"
#include <stdio.h>

static void setMeshIndex(mesh* M, GLuint *indices) {
	glBindVertexArray(M->VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, M->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, M->indexCount * sizeof(GLuint), indices, GL_STATIC_DRAW);
}

static void setVertexData(mesh *M, GLfloat *vertices, const char* texLocation) {
	glBindVertexArray(M->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, M->VBO);
	glBufferData(GL_ARRAY_BUFFER, 8 * M->vertexCount * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);    // Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);   //The starting point of the VBO, for the vertices

	glEnableVertexAttribArray(1);    // Texture
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));

	glEnableVertexAttribArray(2);    // Normal
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *)(5 * sizeof(GLfloat)));

	SDL_Surface *image;
	image=IMG_Load(texLocation);
	if(!image) {
	    printf("IMG_Load: %s\n", IMG_GetError());
	}

	glBindTexture(GL_TEXTURE_2D, M->tex_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->w, image->h, 0, GL_RGB, GL_UNSIGNED_BYTE, image->pixels); // vertices == image data

	SDL_FreeSurface(image);
}

static void initMesh(mesh *M) {
	quat_identity(M->transform.orientation);
	mesh_translate(M, 0, 0, 0);
	mesh_scale(M, 0);
}

mesh* initOBJMesh(const char* filename, const char* texturePath) {
	mesh *M = (mesh*)malloc(sizeof(mesh));

	initMesh(M);

	OBJ_data* data;
	loadOBJ(&data, filename); // TODO: Dynamic allocation for Mesh data arrays

	M->vertexCount = data->vertexCount;
	M->indexCount = data->indexCount;

	vec3_copy(M->hitbox.min, data->hitbox.min);
	vec3_copy(M->hitbox.max, data->hitbox.max);
	
	glGenVertexArrays(1, &(M->VAO));

	glGenBuffers(1, &(M->VBO));
	glGenTextures(1, &(M->tex_id));
	setVertexData(M, data->vertices, texturePath);

	glGenBuffers(1, &(M->EBO));
	setMeshIndex(M, data->indices);

	free(data->indices);
	free(data->vertices);
	free(data);

	return M;
}
int aabb_collision(bounding_box a, bounding_box b) {
	return (a.min[0] < b.max[0] && a.min[1] < b.max[1] && a.min[2] < b.max[2] &&
			b.min[0] < a.max[0] && b.min[1] < a.max[1] && b.min[2] < a.max[2]);
}

void draw(mesh *M) {
	glBindVertexArray(M->VAO);
	glBindTexture(GL_TEXTURE_2D, M->tex_id);
	glDrawElements(GL_TRIANGLES, M->indexCount, GL_UNSIGNED_INT, (void*)0);
	glBindVertexArray(0);
}

void mesh_translate(mesh* M, float x, float y, float z) {
	mat4x4_translate(M->transform.translate, x, y, z);
	M->hitbox.min[0] += x;
	M->hitbox.min[1] += y;
	M->hitbox.min[2] += z;
	M->hitbox.max[0] += x;
	M->hitbox.max[1] += y;
	M->hitbox.max[2] += z;
}

void mesh_rotate_x(mesh* M, float angle) {
	vec3 axis; axis[0] = 1; axis[1] = 0; axis[2] = 0;
	quat_rotate(M->transform.orientation, angle, axis);
}

void mesh_rotate_y(mesh* M, float angle) {
	vec3 axis; axis[0] = 0; axis[1] = 1; axis[2] = 0;
	quat_rotate(M->transform.orientation, angle, axis);
}

void mesh_rotate_z(mesh* M, float angle) {
	vec3 axis; axis[0] = 0; axis[1] = 0; axis[2] = 1;
	quat_rotate(M->transform.orientation, angle, axis);
}

void mesh_scale(mesh* M, float k) {
	mat4x4_scale(M->transform.scale, M->transform.scale, k);
}

void mesh_update_model_matrix(mesh* M) {
	//without scaling
	mat4x4_from_quat(M->transform.rotate, M->transform.orientation);
	mat4x4_mul (M->transform.model, M->transform.translate, M->transform.rotate);
}