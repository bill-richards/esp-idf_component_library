#include <bme_controller.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace gsdc_sensor {
    namespace bme280 {

        bool BmeController::Initialize(uint16_t data_pin, uint16_t clock_pin)
        {
            connected_sensor = new gsdc_sensor::bme280::BME280IIC();

            Gpio_t * pins = connected_sensor->GetGpioSetupStruct();
            pins[gsdc_sensor::bme280::SDA_PIN_INDEX] = Gpio_t { data_pin };
            pins[gsdc_sensor::bme280::SCL_PIN_INDEX] = Gpio_t { clock_pin };

            connected_sensor->SetGpio(pins);
            connected_sensor->Initialize();

            return connected_sensor->IsInitialized();
        }

        void BmeController::ReadData(char * data)
        {
            connected_sensor->ReadData(data);
        }
        
    } // namespace bme280
} // namespace gsdc_sensor

