#include "i2c_master.h"
#include "driver/i2c.h"

#define I2C_MASTER_SDA_IO 21
#define I2C_MASTER_SCL_IO 22
#define I2C_MASTER_NUM 0
#define I2C_MASTER_FREQ_HZ 100000
#define I2C_MASTER_TX_BUF_DISABLE 0
#define I2C_MASTER_RX_BUF_DISABLE 0

int i2c_master_port = 0;

i2c_config_t i2c_config = {
    .mode = I2C_MODE_MASTER,
    .sda_io_num = I2C_MASTER_SDA_IO,
    .sda_pullup_en = GPIO_PULLUP_ENABLE,
    .scl_io_num = I2C_MASTER_SCL_IO,
    .scl_pullup_en = GPIO_PULLUP_ENABLE,
    .master.clk_speed = I2C_MASTER_FREQ_HZ,
    //.clk_flags = 0
};

/**
 * @brief Inicializa el dispositivo I2C como master.
 * 
 */
void i2c_init() {
    i2c_param_config(i2c_master_port, &i2c_config);
    i2c_driver_install(i2c_master_port, i2c_config.mode, I2C_MASTER_TX_BUF_DISABLE, I2C_MASTER_RX_BUF_DISABLE, 0);
}