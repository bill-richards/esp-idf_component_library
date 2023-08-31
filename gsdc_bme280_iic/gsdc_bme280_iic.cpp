#include <stdio.h>
#include <string.h>
#include <esp_logging.h>

#include "gsdc_bme280_iic.h"

const static char * BME280_IIC_TAG = "BME280_IIC";
namespace gsdc_bme280
{
    esp_err_t BME280IIC::writeByteData(const uint8_t reg, const uint8_t value)
    {
        return i2c->WriteRegister(_devAddress, reg, value);
    }

    int BME280IIC::readByteData(const uint8_t reg)
    {
        return i2c->ReadRegister(_devAddress, reg);
    }

    int BME280IIC::readWordData(const uint8_t reg)
    {
        uint8_t buff[2];
        i2c->ReadRegisterMultipleBytes(_devAddress, reg, buff, 2);
        return buff[1] << 8 | buff[0];
    }

    esp_err_t BME280IIC::readBlockData(const uint8_t reg, uint8_t *buf, const int length)
    {
        return i2c->ReadRegisterMultipleBytes(_devAddress, reg, buf, length);
    }

    void BME280IIC::InitIIc(gsdc_cppiic::IIC *i_i2c, const uint8_t dev_addr)
    {
        i2c = i_i2c;
        _devAddress = dev_addr;
    }

    void BME280IIC::ReadData(char data[])
    {
        // strcpy(data, "This is not test data, this is data collected outside of the iic component library, and transmitted by the iic component");
        GetAllResults(&i2cTemperature, &i2cHumidity, &i2cPressure);
        // char results[1024];
        sprintf(data, "Temp[%f],Humidity[%u],Pressure[%f],Id[%x]%s", i2cTemperature, i2cHumidity, i2cPressure, i2cId, "\0");
        // strcpy(data, results);
    }

    void BME280IIC::Initialize(void)
    {
        ESP_LOGI(BME280_IIC_TAG, "Starting the IIC-Client thread ...");

        i2c = new gsdc_cppiic::IIC(I2C_NUM_1);
        i2c->InitMaster(26, 25, 400000, true, true);
        InitIIc(i2c, 0x76);
        Init();
        SetMode(1);
        SetConfigFilter(1);
    }

} // namespace gsdc_bme280
