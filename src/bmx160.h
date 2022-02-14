/**
 * @file bmx160.h
 * @brief Driver del sensor BMX160
 * @version 0.1
 * @date 2022-02-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef BMX160_H
#define BMX160_H

#include "vec3.h"

typedef struct {
    vec3_t acc;
    vec3_t gyr;
    vec3_t mag;
    double hall;
} bmx160_data_t;

void bmx160_init();
bmx160_data_t bmx160_read();

#endif