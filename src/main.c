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

    // Leer e imprimir en pantalla
    while(true) {
        sensor_t sensor = bmx160_read();

        ESP_LOGI(TAG, "Magnetometro en uT. x = %f y = %f z = %f", sensor.mag.x, sensor.mag.y, sensor.mag.z);

        // Convertir vector de campo magnético a orientación
        orientacion_t orientacion;

        orientacion.alpha = atan2f(sensor.mag.x, sensor.mag.y) * 180 / M_PI;

        float h = sqrtf(powf(sensor.mag.x, 2.0f) + powf(sensor.mag.y, 2.0f));
        orientacion.gamma = atan2f(h, sensor.mag.z) * 180 / M_PI;

        ESP_LOGI(TAG, "Orientacion = %f | Inclinacion = %f", orientacion.alpha, orientacion.gamma);

        // Encolo los datos para transmitir
        xStreamBufferSend(xStreamBuffer, (void *) &orientacion, sizeof(orientacion_t), 0);

        vTaskDelay(500 / portTICK_PERIOD_MS);
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