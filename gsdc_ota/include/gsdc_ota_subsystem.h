#ifndef __OTA_SUBSYSTEM_H__
#define __OTA_SUBSYSTEM_H__

#include <freertos/FreeRTOS.h>                  //


#ifdef __cplusplus
extern "C" {
#endif

void gsdc_ota_subsystem_initialize(uint8_t iicAddress);

#ifdef __cplusplus
}
#endif



#endif // __OTA_SUBSYSTEM_H__