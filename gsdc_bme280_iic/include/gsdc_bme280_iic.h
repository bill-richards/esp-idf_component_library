#ifndef __GSDC_BME280_IIC_H__
#define __GSDC_BME280_IIC_H__

#include <gsdc_bme280.h>
#include <gsdc_cppiic.h>

namespace gsdc_bme280
{
    typedef gsdc_telemetry::GSDC_SENSOR GSDC_SENSOR;

    class BME280IIC final : public BME280
    {
    private:
        gsdc_cppiic::IIC *i2c;
        uint8_t _devAddress{};
        float i2cTemperature{};
        float i2cPressure{};
        int i2cHumidity{};
        int i2cId{};

    protected:
        esp_err_t writeByteData(const uint8_t reg, const uint8_t value);
        int readByteData(const uint8_t reg);
        int readWordData(const uint8_t reg);
        esp_err_t readBlockData(const uint8_t reg, uint8_t *buf, const int length);

    public:
        void InitIIc(gsdc_cppiic::IIC *i_i2c, const uint8_t dev_addr = 0x76);
        virtual void ReadData(char data[]) override;
        virtual void Initialize(void) override;
    };
    
} // namespace gsdc_bme280

#endif // __GSDC_BME280_IIC_H__