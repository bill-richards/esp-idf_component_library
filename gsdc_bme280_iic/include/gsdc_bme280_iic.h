#ifndef __GSDC_BME280_IIC_H__
#define __GSDC_BME280_IIC_H__

#include <gsdc_bme280.h>
#include <gsdc_cppiic.h>

namespace gsdc_bme280
{
    typedef gsdc_telemetry::GSDC_SENSOR GSDC_SENSOR;
    typedef gsdc_telemetry::Gpio_t Gpio_t;

    static const int SDA_PIN_INDEX = 0;
    static const int SCL_PIN_INDEX = 1;


    class BME280IIC final : public BME280
    {
    private:
        gsdc_cppiic::IIC *i2c;
        uint8_t _devAddress{};
        float i2cTemperature{};
        float i2cPressure{};
        int i2cHumidity{};
        int i2cId{};
        bool _gpioHasBeenSet = false;
        bool _initialized = false;
        int SDA_PIN{};
        int SCL_PIN{};
    public:
        // static const int SDA_PIN_INDEX = 0;
        // static const int SCL_PIN_INDEX = 1;

    protected:
        esp_err_t writeByteData(const uint8_t reg, const uint8_t value);
        int readByteData(const uint8_t reg);
        int readWordData(const uint8_t reg);
        esp_err_t readBlockData(const uint8_t reg, uint8_t *buf, const int length);

    private:
        void InitIIc(gsdc_cppiic::IIC *i_i2c, const uint8_t dev_addr = 0x76);

    public:
        Gpio_t * GetGpioSetupStruct(void) override;
        void Initialize(void) override;
        bool IsInitialized() override;
        void ReadData(char *) override;
        void SetGpio(Gpio_t pins[]) override;
        
        ~BME280IIC();
    };
    
} // namespace gsdc_bme280

#endif // __GSDC_BME280_IIC_H__