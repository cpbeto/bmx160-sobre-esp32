#include "sensor.h"

void sensor_update(motion_sensor_t sensor, vec3_t acc, vec3_t gyr, vec3_t mag) {
    sensor.acc = acc;
    sensor.gyr = gyr;
    sensor.mag = mag;
    // Calculo la gravedad usando un filtro pasabajos sobre el valor del acelerómetro
    sensor.gravity = vec3_add(vec3_scale(sensor.acc, sensor.alpha), vec3_scale(sensor.gravity, 1 - sensor.alpha));
    // Calculo el vector de orientación norte y este.
    sensor.east = vec3_cross_product(sensor.mag, sensor.gravity);
    sensor.north = vec3_cross_product(sensor.gravity, sensor.east);
}