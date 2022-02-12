/**
 * @file i2c_master.h
 * @brief Inicializa el puerto I2C como master
 * @version 0.1
 * @date 2022-02-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#define I2C_MASTER_TIMEOUT_MS 100

extern int i2c_master_port;
void i2c_init();