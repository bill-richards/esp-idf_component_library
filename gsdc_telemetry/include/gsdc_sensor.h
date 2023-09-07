#ifndef __GSDC_SENSOR_H__
#define __GSDC_SENSOR_H__

namespace gsdc_telemetry
{
    typedef struct {
        int PinNumber;
    } Gpio_t;

    class GSDC_SENSOR
    {
    public:
        virtual bool IsInitialized() = 0;
        virtual void Initialize(void) = 0;
        /**
         * @brief The caller is responsible for freeing the returned char *
         */
        virtual void ReadData(char * ) = 0;
        virtual Gpio_t * GetGpioSetupStruct(void) = 0;
        virtual void SetGpio(Gpio_t pins[]) = 0;
        virtual ~GSDC_SENSOR();
    };


} // namespace gsdc_telemetry

#endif // __GSDC_SENSOR_H__
