/**
 * @file main.c
 * @brief Implementación del sensor BMX160 sobre ESP32
 * @version 0.1
 * @date 2022-02-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/stream_buffer.h"
#include "math.h"
//
#include "bmx160.h"
#include "i2c_master.h"
#include "motion_sensor.h"

// Tag para imprimir por pantalla
static const char *TAG = "bmx160-app";

// Estructura con datos de orientacion
typedef struct {
    float alpha; // heading
    float gamma; // inclination
    // float timestamp; // se podría agregar un timestamp para recrear el movimiento
} orientacion_t;

// Stream buffer con datos de orientacion para transmitir.
StreamBufferHandle_t xStreamBuffer;

/**
 * @brief Task de transmisión por Bluetooth, recibe los datos por un Stream Buffer.
 * No está implementado, solo el prototipo.
 */
void bt_task() {
    // Inicializar Bluetooth
    // @todo

    // Leer el stream y transmitir
    orientacion_t data[10];
    while(true) {
        size_t xReceivedBytes = xStreamBufferReceive(xStreamBuffer, (void *) data, sizeof(orientacion_t) * 10, portMAX_DELAY);
        // assert(xReceivedBytes % sizeof(orientacion_t) == 0);

        // Transmitir datos de orientacion
        // @todo
    }
}

/**
 * @brief Task que lee los datos del magnetómetro del sensor,
 * calcula la orientacion espacial e imprime en pantalla.
 * Además, escribe en un Stream Buffer para alimentar al task de transmisión.
 */
void bmx160_task() {
    // Inicializar sensor BMX160
    bmx160_init();

    // Tomar una primer lectura para inicializar la estructura de sensor compuesto
    bmx160_data_t bmx160_data = bmx160_read();
    motion_sensor_t sensor = sensor_init(bmx160_data.acc, bmx160_data.gyr, bmx160_data.mag, 0.8);

    // Leer e imprimir en pantalla
    while(true) {
        bmx160_data = bmx160_read();

        ESP_LOGI(TAG, "Magnetometro en uT. x = %f y = %f z = %f", bmx160_data.mag.x, bmx160_data.mag.y, bmx160_data.mag.z);

        sensor_update(&sensor, bmx160_data.acc, bmx160_data.gyr, bmx160_data.mag);

        // @todo Transformar datos del sensor a orientacion e inclinacion (heading & inclination).
        orientacion_t orientacion = {0.0, 0.0};
        ESP_LOGI(TAG, "Orientacion = %f | Inclinacion = %f", orientacion.alpha, orientacion.gamma);

        // Encolo los datos para transmitir
        xStreamBufferSend(xStreamBuffer, (void *) &orientacion, sizeof(orientacion_t), 0);

        // Muestrear a 10 Hz
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief Aplicacion principal (main entry point)
 * 
 */
void app_main() {
    // Inicializar I2C
    i2c_init();

    // Crear stream para la transmisión de orientación espacial
    xStreamBuffer = xStreamBufferCreate(sizeof(orientacion_t) * 256, sizeof(orientacion_t) * 1);

    // Ejecutar tarea de lectura del sensor
    xTaskCreate(bmx160_task, "bmx160_task", 2048, (void *)0, 10, NULL);
    
    // Ejecutar tarea de transmisión de datos
    xTaskCreate(bt_task, "bt_task", 2048, (void *)0, 10, NULL);
}