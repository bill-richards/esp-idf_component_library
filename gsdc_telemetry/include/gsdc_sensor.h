#ifndef __GSDC_SENSOR_H__
#define __GSDC_SENSOR_H__

namespace gsdc_telemetry
{

    class GSDC_SENSOR
    {
    public:
        virtual void ReadData(char data[]) = 0;
        virtual void Initialize(void) = 0;
        virtual ~GSDC_SENSOR();
    };


} // namespace gsdc_telemetry

#endif // __GSDC_SENSOR_H__
