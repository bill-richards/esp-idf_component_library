#ifndef __BME_CONTROLLER_H__
#define __BME_CONTROLLER_H__

#include <gsdc_bme280_iic.h>

typedef gsdc_sensor::bme280::Gpio_t Gpio_t;

namespace gsdc_sensor {
    namespace bme280 { 

        class BmeController {
        private:
            gsdc_sensor::bme280::GSDC_SENSOR * connected_sensor = nullptr;
        public:
            bool Initialize(uint16_t data_pin, uint16_t clock_pin);
            /**
             * @brief The caller is responsible for freeing the returned char *
             */
            void ReadData(char *);
        };


    } // namespace bme280
} // namespace gsdc_sensor

#endif // __BME_CONTROLLER_H__