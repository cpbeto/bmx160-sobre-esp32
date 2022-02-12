#include "bmx160.h"

#include "i2c_master.h"
#include "driver/i2c.h"
#include "freertos/task.h"

#define BMX160_SLAVE_ADDR 0x68

#define BMX160_REG_DATA 0x04
#define BMX160_REG_MAG_CONF 0x44
#define BMX160_REG_MAG_IF_0 0x4c
#define BMX160_REG_MAG_IF_1 0x4d
#define BMX160_REG_MAG_IF_2 0x4e
#define BMX160_REG_MAG_IF_3 0x4f
#define BMX160_REG_CMD 0x7e

#define BMX160_CMD_SOFT_RESET 0xb6
#define BMX160_CMD_MAG_NORMAL 0x19
#define BMX160_CMD_MAG_LOW_POWER 0x1A

#define BMX160_MAG_RES_X (0.0351f) // +- 1150 uT / 16 bits
#define BMX160_MAG_RES_Y (0.0351f) // +- 1150 uT / 16 bits
#define BMX160_MAG_RES_Z (0.0763f) // +- 2500 uT / 16 bits

/**
 * @brief Escribe un registro n (1 byte) en el sensor
 * 
 * @param addr Dirección del registro
 * @param byte Dato a escribir
 */
void bmx160_write(uint8_t addr, uint8_t byte) {
    i2c_cmd_handle_t cmd_handle = i2c_cmd_link_create();
    
    i2c_master_start(cmd_handle);
    i2c_master_write_byte(cmd_handle, BMX160_SLAVE_ADDR, true);
    i2c_master_write_byte(cmd_handle, addr, true);
    i2c_master_write_byte(cmd_handle, byte, true);
    i2c_master_stop(cmd_handle);
    
    i2c_master_cmd_begin(i2c_master_port, cmd_handle, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);

    i2c_cmd_link_delete(cmd_handle);
}

/**
 * @brief Ejecuta un comando en el sensor
 * 
 * @param cmd Comando a ejecutar
 */
void bmx160_write_cmd(uint8_t cmd) {
    bmx160_write(BMX160_REG_CMD, cmd);
}

/**
 * @brief Inicializa el sensor BMX160, actualmente solo se utiliza el magnetómetro.
 * 
 */
void bmx160_init() {    
    // Soft reset del sensor
    bmx160_write_cmd(BMX160_CMD_SOFT_RESET);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    // Habilitar magnetómetro
    bmx160_write_cmd(BMX160_CMD_MAG_NORMAL);
    vTaskDelay(10 / portTICK_PERIOD_MS);

    // Configurar magnetómetro (pag. 25 datasheet)
    bmx160_write(BMX160_REG_MAG_IF_0, 0x80);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    // Sleep mode
    bmx160_write(BMX160_REG_MAG_IF_3, 0x01);
    bmx160_write(BMX160_REG_MAG_IF_2, 0x4b);
    // REPXY regular preset
    bmx160_write(BMX160_REG_MAG_IF_3, 0x04);
    bmx160_write(BMX160_REG_MAG_IF_2, 0x51);
    // REPZ regular preset
    bmx160_write(BMX160_REG_MAG_IF_3, 0x0E);
    bmx160_write(BMX160_REG_MAG_IF_2, 0x52);
    // Prepare for data mode
    bmx160_write(BMX160_REG_MAG_IF_3, 0x02);
    bmx160_write(BMX160_REG_MAG_IF_2, 0x4C);
    bmx160_write(BMX160_REG_MAG_IF_1, 0x42);
    // Frecuencia de trabajo 12.5Hz
    bmx160_write(BMX160_REG_MAG_CONF, 0x05);
    // Data mode
    bmx160_write(BMX160_REG_MAG_IF_0, 0x00);
    // Pasar a low-power
    bmx160_write_cmd(BMX160_CMD_MAG_NORMAL);
    vTaskDelay(10 / portTICK_PERIOD_MS);
}

/**
 * @brief Lee los datos brutos del sensor
 * 
 * @param data Buffer de lectura
 */
void bmx160_read_data(uint8_t *data) {
    i2c_cmd_handle_t cmd_handle = i2c_cmd_link_create();
    
    // Registro a leer
    i2c_master_start(cmd_handle);
    i2c_master_write_byte(cmd_handle, BMX160_SLAVE_ADDR, true);
    i2c_master_write_byte(cmd_handle, BMX160_REG_DATA, true);
    
    // Realizar lectura
    i2c_master_start(cmd_handle);
    i2c_master_write_byte(cmd_handle, BMX160_SLAVE_ADDR, true);
    i2c_master_read(cmd_handle, data, 5, I2C_MASTER_ACK);
    i2c_master_read(cmd_handle, data + 5, 1, I2C_MASTER_NACK);
    i2c_master_stop(cmd_handle);

    i2c_master_cmd_begin(i2c_master_port, cmd_handle, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);

    i2c_cmd_link_delete(cmd_handle);
}

/**
 * @brief Lee los datos del sensor (procesados y convertidos)
 * 
 * @return sensor_t Datos leidos
 */
sensor_t bmx160_read() {
    // Leer datos del sensor
    uint8_t data[6];
    bmx160_read_data(data);

    // Convertir datos a estructura
    sensor_t sensor;

    int16_t x, y, z;

    // Magnetómetro
    x = (int16_t) (((uint16_t)data[1] << 8) | data[0]);
    y = (int16_t) (((uint16_t)data[3] << 8) | data[2]);
    z = (int16_t) (((uint16_t)data[5] << 8) | data[4]);

    sensor.mag.x = x * BMX160_MAG_RES_X;
    sensor.mag.y = y * BMX160_MAG_RES_Y;
    sensor.mag.z = z * BMX160_MAG_RES_Z;

    return sensor;
}