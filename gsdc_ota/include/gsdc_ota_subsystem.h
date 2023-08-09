#ifndef __OTA_SUBSYSTEM_H__
#define __OTA_SUBSYSTEM_H__

#include <freertos/FreeRTOS.h>                  //
#include "gsdc_ota.h"

#ifdef __cplusplus
extern "C" {
#endif


void gsdc_ota_subsystem_initialize(gsdc_ota_configuration_file_descriptor_t * configurationDescriptor);


#ifdef __cplusplus
}
#endif



#endif // __OTA_SUBSYSTEM_H__