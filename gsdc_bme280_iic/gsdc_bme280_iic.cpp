#include <stdio.h>
#include <string.h>
#include <esp_logging.h>
#include <gsdc_string_utils.h>

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

    void BME280IIC::InitIIc(gsdc_cppiic::IIC * i_i2c, const uint8_t dev_addr)
    {
        i2c = i_i2c;
        _devAddress = dev_addr;
    }

    void BME280IIC::ReadData(char * data)
    {
        if(!_initialized)
        {
            ESP_LOGE(BME280_IIC_TAG, "Sensor must be initialized before use");
            return;
        }

        GetAllResults(&i2cTemperature, &i2cHumidity, &i2cPressure);
        sprintf(data, "T:%.2f|H:%u|P:%f%s", i2cTemperature, i2cHumidity, i2cPressure, "\0");
        return;
    }

    void BME280IIC::Initialize(void)
    {
        ESP_LOGI(BME280_IIC_TAG, "Initializing the BME280 Sensor ...");

        if(!_gpioHasBeenSet)
        {
            ESP_LOGE(BME280_IIC_TAG, "GPIO must be set before initialization");
            return;
        }

        i2c = new gsdc_cppiic::IIC(I2C_NUM_1);
        i2c->InitMaster(SDA_PIN, SCL_PIN, 400000, true, true);
        InitIIc(i2c, 0x76);
        Init();
        SetMode(1);
        SetConfigFilter(1);
        _initialized = true;
    }

    bool BME280IIC::IsInitialized() { return _initialized; }

    Gpio_t * BME280IIC::GetGpioSetupStruct(void)
    {
        Gpio_t * pins = (Gpio_t *)calloc(2, sizeof(Gpio_t));
        return pins;
    }

    void BME280IIC::SetGpio(Gpio_t * pins)
    {
        ESP_LOGI(BME280_IIC_TAG, "Setting up Gpio ...");
        SDA_PIN = pins[SDA_PIN_INDEX].PinNumber; // 26
        SCL_PIN = pins[SCL_PIN_INDEX].PinNumber; // 25
        free(pins);

        _gpioHasBeenSet = true;
    }

    BME280IIC::~BME280IIC()
    {
        free(i2c);
    }

} // namespace gsdc_bme280
