#include "../include/light.h"
#include "../include/utils.h"

void initAmbientLight(ambientLight* ambient, vec3 color, float intensity) {
	vec3_copy(ambient->color, color);
	ambient->intensity = intensity;
}

void setAmbientLight(ambientLight* ambient, shader *S) {
	glUniform3fv(S->location.ambientLightColor, 1, (GLfloat*)ambient->color);
	glUniform1f(S->location.ambientLightIntensity, ambient->intensity);
}

void initPointLight(pointLight* point, vec3 color, vec3 pos, float att) {
	vec3_copy(point->color, color);
	vec3_copy(point->position, pos);
	point->attenuation = att;
}

void setPointLight(pointLight* point, shader *S) {
	glUniform3fv(S->location.pointLightPosition, 1, (GLfloat*)point->position);
	glUniform3fv(S->location.pointLightColor, 1, (GLfloat*)point->color);
	glUniform1f(S->location.attenuation, point->attenuation);
}