#ifndef __GSDC_OTA_H__
#define __GSDC_OTA_H__

#include <esp_err.h>

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t gsdc_ota_configure_wifi(char * ssid);
esp_err_t http_server_init(void);

#ifdef __cplusplus
}
#endif

#endif // __GSDC_OTA_H__