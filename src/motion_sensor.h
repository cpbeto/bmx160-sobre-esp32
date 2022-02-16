/**
 * @file sensor.h
 * @brief Implementación de un sensor de movimiento
 * siguiendo los lineamientos de https://www.w3.org/TR/motion-sensors/
 * y otras refs.
 * 
 * Los vectores de gravedad y aceleración lineal usan el mismo sistema de
 * referencia que el sensor básico de 9DOF (acelerómetro, giroscopio y magnetómetro).
 * 
 * Esto quiere decir que el valor de dichos vectores es relativo al sensor.
 * 
 * Lo mismo ocurre con los vectores del compás: norte y este.
 * 
 * La orientación absoluta, en cambio, se mide usando un sistema de referencia
 * que es absoluto (i.e. independiente del sensor):
 * x = Dirección Este
 * y = Dirección Norte
 * z = Dirección vertical hacia "arriba" (siguiendo el radio terrestre)
 * 
 * Los valores de orientación absoluta (en sistema de Euler o cuaterniones) van a estar
 * referidos a este sistema de referencia que, para cualquier punto de la superficie terrestre,
 * es invariante.
 * 
 * @version 0.1
 * @date 2022-02-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef MOTION_SENSOR_H
#define MOTION_SENSOR_H

#include "vec3.h"

typedef struct {
    // Sensores básicos
    vec3_t acc;
    vec3_t gyr;
    vec3_t mag;
    // Sensor de gravedad
    double alpha;
    vec3_t gravity;
    // Sensor de orientación
    vec3_t north;
    vec3_t east;
    double heading;
    double inclination;
} motion_sensor_t;

motion_sensor_t sensor_init(vec3_t acc, vec3_t gyr, vec3_t mag, double alpha);
void sensor_update(motion_sensor_t *sensor, vec3_t acc, vec3_t gyr, vec3_t mag);

#endif
