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
#define BMX160_CMD_ACC_NORMAL 0x11
#define BMX160_CMD_GYR_NORMAL 0x15
#define BMX160_CMD_MAG_NORMAL 0x19
#define BMX160_CMD_MAG_LOW_POWER 0x1A

#define BMX160_MAG_RES_X 0.0351 // +- 1150 uT / 16 bits
#define BMX160_MAG_RES_Y 0.0351 // +- 1150 uT / 16 bits
#define BMX160_MAG_RES_Z 0.0763 // +- 2500 uT / 16 bits

#define BMX160_ACC_RES 0.000061035 // +- 2g / 16 bits

#define BMX160_GYR_RES 0.061035 // +- 2000dps / 16 bits

#define BMX160_DATA_SIZE 20 // Tamaño del buffer de datos

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

    // Habilitar acelerómetro
    bmx160_write_cmd(BMX160_CMD_ACC_NORMAL);
    vTaskDelay(10 / portTICK_PERIOD_MS);

    // Habilitar giroscopio
    bmx160_write_cmd(BMX160_CMD_GYR_NORMAL);
    vTaskDelay(10 / portTICK_PERIOD_MS);

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
 * @brief Lee los datos en crudo del sensor
 * 
 * @param buffer Buffer donde guardar los datos (tamaño mínimo BMX160_DATA_SIZE)
 */
void bmx160_read_raw(uint8_t *buffer) {
    i2c_cmd_handle_t cmd_handle = i2c_cmd_link_create();
    
    // Registro a leer
    i2c_master_start(cmd_handle);
    i2c_master_write_byte(cmd_handle, BMX160_SLAVE_ADDR, true);
    i2c_master_write_byte(cmd_handle, BMX160_REG_DATA, true);
    
    // Realizar lectura
    i2c_master_start(cmd_handle);
    i2c_master_write_byte(cmd_handle, BMX160_SLAVE_ADDR, true);
    i2c_master_read(cmd_handle, buffer, BMX160_DATA_SIZE - 1, I2C_MASTER_ACK);
    i2c_master_read(cmd_handle, buffer + BMX160_DATA_SIZE - 1, 1, I2C_MASTER_NACK);
    i2c_master_stop(cmd_handle);

    i2c_master_cmd_begin(i2c_master_port, cmd_handle, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);

    i2c_cmd_link_delete(cmd_handle);
}

/**
 * @brief Lee los datos del sensor (procesados y convertidos)
 * 
 * @return sensor_t Datos leidos
 */
bmx160_data_t bmx160_read() {
    // Leer datos crudos del sensor
    uint8_t buffer[BMX160_DATA_SIZE];
    bmx160_read_raw(buffer);

    // Convertir datos a estructura
    bmx160_data_t data;

    int16_t x, y, z;

    // Magnetómetro
    x = (int16_t) (((uint16_t)buffer[1] << 8) | buffer[0]);
    y = (int16_t) (((uint16_t)buffer[3] << 8) | buffer[2]);
    z = (int16_t) (((uint16_t)buffer[5] << 8) | buffer[4]);

    data.mag.x = x * BMX160_MAG_RES_X;
    data.mag.y = y * BMX160_MAG_RES_Y;
    data.mag.z = z * BMX160_MAG_RES_Z;

    // Sensor Hall
    // @todo
    data.hall = 0.0;

    // Magnetómetro
    x = (int16_t) (((uint16_t)buffer[9] << 8) | buffer[8]);
    y = (int16_t) (((uint16_t)buffer[11] << 8) | buffer[10]);
    z = (int16_t) (((uint16_t)buffer[13] << 8) | buffer[12]);

    data.mag.x = x * BMX160_ACC_RES;
    data.mag.y = y * BMX160_ACC_RES;
    data.mag.z = z * BMX160_ACC_RES;

    // Magnetómetro
    x = (int16_t) (((uint16_t)buffer[15] << 8) | buffer[14]);
    y = (int16_t) (((uint16_t)buffer[17] << 8) | buffer[16]);
    z = (int16_t) (((uint16_t)buffer[19] << 8) | buffer[18]);

    data.mag.x = x * BMX160_GYR_RES;
    data.mag.y = y * BMX160_GYR_RES;
    data.mag.z = z * BMX160_GYR_RES;

    return data;
}