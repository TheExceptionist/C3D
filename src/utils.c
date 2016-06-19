#include "../include/utils.h"
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>

void vec4_copy(vec4 r, vec4 a) {
    int i;
    for (i = 0; i < 4; r[i] = a[i++]);
}

void vec3_copy(vec3 r, vec3 a) {
    int i;
    for (i = 0; i < 3; r[i] = a[i++]);
}

int readfile(char** s, const char* filename) {
    struct stat st;
    int i;

    if(stat(filename, &st) == -1)
        return 0;

    *s = malloc(st.st_size+2);
    memset (*s, 0, st.st_size+2); // \0

    FILE *f;
    f = fopen(filename,"rb");
    fread(*s, 1, st.st_size, f);
    
    return 1;
}

void mat4x4_debug_print(mat4x4 a) {
    int i, j;
    printf("------------------------------------\n");
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++)
            printf("%f ", a[i][j]);
        putchar('\n');
    }
    printf("------------------------------------\n");
}

void mat4x4_mult(mat4x4 a, mat4x4 b, mat4x4 r) {
    int i, j, k;
    float sum;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++) {
            r[i][j] = 0;
            for (k = 0; k < 4; k++)
                r[i][j] += a[k][j] * b[i][k];
        }
}

void mat4x4_gen_scale(mat4x4 a, float x, float y, float z) {
    a[0][0] = x;    a[0][1] = 0;    a[0][2] = 0;    a[0][3] = 0;
    a[1][0] = 0;    a[1][1] = y;    a[1][2] = 0;    a[1][3] = 0;
    a[2][0] = 0;    a[2][1] = 0;    a[2][2] = z;    a[2][3] = 0;
    a[3][0] = 0;    a[3][1] = 0;    a[3][2] = 0;    a[3][3] = 1;
}

void mat4x4_gen_translate(mat4x4 a, float x, float y, float z) {
    a[0][0] = 1;    a[0][1] = 0;    a[0][2] = 0;    a[0][3] = 0;
    a[1][0] = 0;    a[1][1] = 1;    a[1][2] = 0;    a[1][3] = 0;
    a[2][0] = 0;    a[2][1] = 0;    a[2][2] = 1;    a[2][3] = 0;
    a[3][0] = x;    a[3][1] = y;    a[3][2] = z;    a[3][3] = 1;
}

void mat4x4_gen_rotate(mat4x4 a, short y, short x, short z, float angle) {
    angle = toRadian(angle);
    if (x) {
        a[0][0] = 1;            a[0][1] = 0;            a[0][2] = 0;            a[0][3] = 0;
        a[1][0] = 0;            a[1][1] = cos(angle);   a[1][2] = -sin(angle);  a[1][3] = 0;
        a[2][0] = 0;            a[2][1] = sin(angle);   a[2][2] =  cos(angle);  a[2][3] = 0;
        a[3][0] = 0;            a[3][1] = 0;            a[3][2] = 0;            a[3][3] = 1;
    }
    else if(y) {
        a[0][0] = cos(angle);   a[0][1] = 0;            a[0][2] = sin(angle);   a[0][3] = 0;
        a[1][0] = 0;            a[1][1] = 1;            a[1][2] = 0;            a[1][3] = 0;
        a[2][0] = -sin(angle);  a[2][1] = 0;            a[2][2] = cos(angle);   a[2][3] = 0;
        a[3][0] = 0;            a[3][1] = 0;            a[3][2] = 0;            a[3][3] = 1;
    }
    else if(z) {
        a[0][0] = cos(angle);   a[0][1] = -sin(angle);  a[0][2] = 0;    a[0][3] = 0;
        a[1][0] = sin(angle);   a[1][1] =  cos(angle);  a[1][2] = 0;    a[1][3] = 0;
        a[2][0] = 0;            a[2][1] = 0;            a[2][2] = 1;    a[2][3] = 0;
        a[3][0] = 0;            a[3][1] = 0;            a[3][2] = 0;    a[3][3] = 1;
    }
    else
        printf("[DEBUG] WARNING: mat4x4_gen_rotate() callled without defining axis of rotation.\n");
    // TODO: allow rotating on 3 axis calling the function once
}

void mat4x4_gen_orthographic_projection(mat4x4 a, int WIDTH, int HEIGHT, float zFar, float zNear) {

        a[0][0] = 1.0f/WIDTH;   a[0][1] = 0;            a[0][2] = 0;                    a[0][3] = 0;
        a[1][0] = 0;            a[1][1] =  1.0f/HEIGHT; a[1][2] = 0;                    a[1][3] = 0;
        a[2][0] = 0;            a[2][1] = 0;            a[2][2] = -2.0f/(zFar-zNear);   a[2][3] = -(zFar + zNear)/(zFar - zNear);
        a[3][0] = 0;            a[3][1] = 0;            a[3][2] = 0;                    a[3][3] = 1;
        // TODO: perspective projection
}