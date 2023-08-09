#ifndef __OTA_SUBSYSTEM_H__
#define __OTA_SUBSYSTEM_H__

#include <freertos/FreeRTOS.h>                  //
#include "gsdc_ota.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief
 * 
 * @param configurationDescriptor (gsdc_ota_configuration_file_descriptor_t *)
 */
void gsdc_ota_subsystem_initialize(gsdc_ota_configuration_file_descriptor_t * configurationDescriptor);

/**
 * @brief Save the binary image to the next OTA partition and re-boots the ESP32 module
 * 
 * @param image (char *) the new firmware image
 * @param length (size_t) the size of the new image
 */
void gsdc_ota_upload_firmware(const char * image, size_t length);

#ifdef __cplusplus
}
#endif



#endif // __OTA_SUBSYSTEM_H__