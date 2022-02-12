/**
 * @file bmx160.h
 * @brief Driver del sensor BMX160
 * @version 0.1
 * @date 2022-02-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

typedef struct {
    float x;
    float y;
    float z;
} coord_t;

typedef struct {
    coord_t acc;
    coord_t gyr;
    coord_t mag;
} sensor_t;

void bmx160_init();
sensor_t bmx160_read();