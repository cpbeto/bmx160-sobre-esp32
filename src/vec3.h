/**
 * @file vec3.h
 * @brief Librería de vectores double de 3 dimensiones 
 * @version 0.1
 * @date 2022-02-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef VEC3_H
#define VEC3_H

typedef struct {
    double x;
    double y;
    double z;
} vec3_t;

double vec3_norm(vec3_t v);
vec3_t vec3_normalize(vec3_t v);

vec3_t vec3_add(vec3_t v1, vec3_t v2);
vec3_t vec3_scale(vec3_t v, double alpha);

double vec3_scalar_product(vec3_t v1, vec3_t v2);
vec3_t vec3_cross_product(vec3_t v1, vec3_t v2);

#endif