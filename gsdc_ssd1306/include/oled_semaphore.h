#ifndef __OLED_SEMAPHORE_H__
#define __OLED_SEMAPHORE_H__

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

namespace gsdc_oled { 

    SemaphoreHandle_t oled_mux = NULL;

    void take_display_semaphore() 
    {
        if(oled_mux == NULL)
            oled_mux = xSemaphoreCreateMutex();

        xSemaphoreTake(oled_mux, portMAX_DELAY);
    }

    void give_display_semaphore()
    {
        xSemaphoreGive(oled_mux);
    }

} // namespace gsdc_oled 

#endif // __OLED_SEMAPHORE_H__