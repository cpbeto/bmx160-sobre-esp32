#include "motion_sensor.h"

motion_sensor_t sensor_init(vec3_t acc, vec3_t gyr, vec3_t mag, double alpha) {
    motion_sensor_t sensor = {
        acc,
        gyr,
        mag,
        alpha,
        acc,
        vec3_zero(),
        vec3_zero(),
        0.0,
        0.0
    };
    sensor_update(&sensor, acc, gyr, mag);
    return sensor;
}

void sensor_update(motion_sensor_t *sensor, vec3_t acc, vec3_t gyr, vec3_t mag) {
    sensor->acc = acc;
    sensor->gyr = gyr;
    sensor->mag = mag;
    
    // Calculo la gravedad usando un filtro pasabajos sobre el valor del acelerómetro
    sensor->gravity = vec3_add(
        vec3_scale(sensor->acc, sensor->alpha), 
        vec3_scale(sensor->gravity, 1 - sensor->alpha)
    );
    // Calculo los vectores de orientación norte y este.
    sensor->east = vec3_cross_product(sensor->mag, sensor->gravity);
    sensor->north = vec3_cross_product(sensor->gravity, sensor->east);

    // La tupla (este, norte, gravedad) determina un sistema de referencia fijo
    // en la vecindad de un punto de la superficie terrestre.
    // Hacemos un cambio de coordenadas para obtener la orientación absoluta
    // del dispositivo respecto a dicho sistema.
    // @todo
    //double beta_x = vec3_cos(vec3_x(), sensor->gravity);
    //double beta_y = vec3_cos(vec3_y(), sensor->gravity);
    //double beta_z = vec3_cos(vec3_z(), sensor->gravity);

    
    // assert(vec3_scalar_product(sensor->east, sensor->north) == 0.0);
    sensor->heading = vec3_angle(vec3_y(), sensor->north);
    sensor->inclination = vec3_angle(vec3_z(), sensor->gravity);
}
