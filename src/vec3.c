#include "vec3.h"
#include "math.h"

double vec3_norm(vec3_t v) {
    return pow(v.x, 2.0) + pow(v.y, 2.0) + pow(v.z, 2.0);
}

vec3_t vec3_normalize(vec3_t v) {
    return vec3_scale(v, 1.0 / vec3_norm(v));
}

vec3_t vec3_add(vec3_t v1, vec3_t v2) {
    return (vec3_t) {
        v1.x + v2.x,
        v1.y + v2.y,
        v1.z + v2.z
    };
}

vec3_t vec3_scale(vec3_t v, double alpha) {
    return (vec3_t) {
        v.x * alpha,
        v.y * alpha,
        v.z * alpha
    };
}

double vec3_scalar_product(vec3_t v1, vec3_t v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

vec3_t vec3_cross_product(vec3_t v1, vec3_t v2) {
    return (vec3_t) {
        v1.y * v2.z - v1.z * v2.y,
        v1.z * v2.x - v1.x * v2.z,
        v1.x * v2.y - v1.y * v2.x
    };
}