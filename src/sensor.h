/**
 * @file sensor.h
 * @brief Implementación de un sensor de movimiento
 * siguiendo los lineamientos de https://www.w3.org/TR/motion-sensors/
 * @version 0.1
 * @date 2022-02-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "vec3.h"

typedef struct {
    // Sensores básicos
    vec3_t acc;
    vec3_t gyr;
    vec3_t mag;
    // Sensor de gravedad
    float alpha;
    vec3_t gravity;
    // Sensor de orientación
    vec3_t north;
    vec3_t east;
} motion_sensor_t;

void sensor_update(motion_sensor_t sensor, vec3_t acc, vec3_t gyr, vec3_t mag);