#include <ssd1306_controller.h>
#include <oled_semaphore.h>

#include "esp_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char * OLED_TAG = "SSD1306Controller";

namespace gsdc_oled {

    const char time_format_string[] = "%03i     %02i:%02i:%02i";
    const char time_label[] = "UP TIME        ";

    char * time; 
    int seconds{}, minutes{}, hours{}, days{};

    void display_time_task(void * parameters)
    {
        auto oledScreen = (SSD1306_t *)parameters;
        TickType_t lastTime = xTaskGetTickCount();
        while(true)
        {
            vTaskDelayUntil(&lastTime, pdMS_TO_TICKS(100));
            take_display_semaphore();
            ssd1306_display_text(oledScreen, BANNER_1, (char *)&time_label, strlen(time_label), false);
            ssd1306_display_text(oledScreen, BANNER_2, time, strlen(time), false);
            give_display_semaphore();
        }
    }

    void calculate_up_time_task(void * parameters)
    {
        auto width = (int)parameters;
        time = (char *)calloc(width, sizeof(char));
        sprintf(time, time_format_string, days, hours, minutes, seconds);

        TickType_t lastTime = xTaskGetTickCount();
        while(true)
        {
            vTaskDelayUntil(&lastTime, pdMS_TO_TICKS(1000));
            seconds +=1;
            if(seconds %60 == 0) { 
                seconds = 0; minutes+=1; 
                if(minutes %60 == 0) { 
                    minutes = 0; hours+=1; 
                    if(hours %24 == 0) { hours = 0; days+=1;}
                }
            }
            sprintf(time, time_format_string, days, hours, minutes, seconds);
        }
    }

    TaskHandle_t scrolling_lines[7][1] {
        {NULL},
        {NULL},
        {NULL},
        {NULL},
        {NULL},
        {NULL},
        {NULL},
    };

    typedef struct 
    {
        SSD1306_t * pOledScreen;
        char * pText;
        int textLength{0};
        int lineNumber{0};
        bool invertText{false};
        bool continuous{false};
        SPEED scrollSpeed{SPEED::MEDIUM};
    } scroll_line_info_t;


    void scroll_line_task(void * parameters)
    {
        auto scrollLineInfo = (scroll_line_info_t*)(parameters);
        SSD1306_t * pScreen = scrollLineInfo->pOledScreen;
        const char blankScreenLine[] = "                 ";
        char * displayText = (char*)calloc(SSD1306Controller::DISPLAY_WIDTH, sizeof(char));
        int textLength = scrollLineInfo->textLength;
        const int scrollingMessageLength = textLength + SSD1306Controller::DISPLAY_WIDTH;
        char * output_text = (char *)calloc(scrollingMessageLength + 1, sizeof(char));
      
        sprintf(output_text, "                %s", scrollLineInfo->pText);
        
        SPEED scrollSpeed = scrollLineInfo->scrollSpeed;
        int lineNumber = scrollLineInfo->lineNumber;
        bool invertText = scrollLineInfo->invertText;
        bool continuousScrolling = scrollLineInfo->continuous;

        do {

            int index = 0;
            while(index <= scrollingMessageLength)
            {
                sprintf(displayText, "%1.16s", &output_text[index]);
                take_display_semaphore();
                ssd1306_display_text(pScreen, lineNumber, displayText, SSD1306Controller::DISPLAY_WIDTH, invertText);
                give_display_semaphore();
                vTaskDelay(scrollSpeed);
                index++;
            }

            if(!continuousScrolling)
                scrolling_lines[lineNumber][0] = NULL;

        } while (scrolling_lines[lineNumber][0] != NULL);

        take_display_semaphore();
        ssd1306_display_text(pScreen, lineNumber, (char*)blankScreenLine, strlen(blankScreenLine), invertText);
        give_display_semaphore();

        free(displayText);
        free(scrollLineInfo);
        free(output_text);
        vTaskDelete(scrolling_lines[lineNumber][0]);
    }

    void SSD1306Controller::DisplayUpTime(void) 
    {
        xTaskCreatePinnedToCore(calculate_up_time_task, "calculate_up_time_task", 2048, (void*)_oledScreen._width, 1, NULL, 0);
        xTaskCreatePinnedToCore(display_time_task, "display_time_task", 2048, &_oledScreen, 2, NULL, 0);
    }

    bool SSD1306Controller::Initialize(bool flipDisplay, int16_t data_pin, int16_t clock_pin, i2c_port_t i2cPort, int16_t reset_pin) {

        gsdc_ssd1306_i2c_master_init(&_oledScreen, data_pin, clock_pin, reset_pin, i2cPort);
        if(flipDisplay) {
            _oledScreen._flip = true;
            ESP_LOGW(OLED_TAG, "Flip upside down");
        }

        ssd1306_init(&_oledScreen, 128, 64);
        ssd1306_contrast(&_oledScreen, 0xff);
        ssd1306_clear_screen(&_oledScreen, false);

        // Display Count Down
        //uint8_t image[24];
        // memset(image, 0, sizeof(image));
        // ssd1306_display_image(&oledScreen, top, (6*8-1), image, sizeof(image));
        // ssd1306_display_image(&oledScreen, top+1, (6*8-1), image, sizeof(image));
        // ssd1306_display_image(&oledScreen, top+2, (6*8-1), image, sizeof(image));
       
        // for(int font=0x35;font>0x30;font--) {
        //     memset(image, 0, sizeof(image));
        //     ssd1306_display_image(&oledScreen, top+1, (7*8-1), image, 8);
        //     memcpy(image, font8x8_basic_tr[font], 8);
        //     if (oledScreen._flip) ssd1306_flip(image, 8);
        //     ssd1306_display_image(&oledScreen, top+1, (7*8-1), image, 8);
        //     vTaskDelay(1000 / portTICK_PERIOD_MS);
        // }
        // ssd1306_clear_screen(&oledScreen, false);

        // DisplayUpTime();
	
        // Scroll Up
        // ssd1306_clear_screen(&oledScreen, false);
        // ssd1306_contrast(&oledScreen, 0xff);
        // ssd1306_display_text(&oledScreen, 0, "---Scroll  UP---", 16, true);
        // //ssd1306_software_scroll(&oledScreen, 7, 1);
        // ssd1306_software_scroll(&oledScreen, (oledScreen._pages - 1), 1);
        // for (int line=0;line<bottom+10;line++) {
        //     lineChar[0] = 0x01;
        //     sprintf(&lineChar[1], " Line %02d", line);
        //     ssd1306_scroll_text(&oledScreen, lineChar, strlen(lineChar), false);
        //     vTaskDelay(500 / portTICK_PERIOD_MS);
        // }
        // vTaskDelay(3000 / portTICK_PERIOD_MS);
        
        // // Scroll Down
        // ssd1306_clear_screen(&oledScreen, false);
        // ssd1306_contrast(&oledScreen, 0xff);
        // ssd1306_display_text(&oledScreen, 0, "--Scroll  DOWN--", 16, true);
        // //ssd1306_software_scroll(&oledScreen, 1, 7);
        // ssd1306_software_scroll(&oledScreen, 1, (oledScreen._pages - 1) );
        // for (int line=0;line<bottom+10;line++) {
        //     lineChar[0] = 0x02;
        //     sprintf(&lineChar[1], " Line %02d", line);
        //     ssd1306_scroll_text(&oledScreen, lineChar, strlen(lineChar), false);
        //     vTaskDelay(500 / portTICK_PERIOD_MS);
        // }
        // vTaskDelay(3000 / portTICK_PERIOD_MS);

        // // Page Down
        // ssd1306_clear_screen(&oledScreen, false);
        // ssd1306_contrast(&oledScreen, 0xff);
        // ssd1306_display_text(&oledScreen, 0, "---Page	DOWN---", 16, true);
        // ssd1306_software_scroll(&oledScreen, 1, (oledScreen._pages-1) );
        // for (int line=0;line<bottom+10;line++) {
        //     //if ( (line % 7) == 0) ssd1306_scroll_clear(&oledScreen);
        //     if ( (line % (oledScreen._pages-1)) == 0) ssd1306_scroll_clear(&oledScreen);
        //     lineChar[0] = 0x02;
        //     sprintf(&lineChar[1], " Line %02d", line);
        //     ssd1306_scroll_text(&oledScreen, lineChar, strlen(lineChar), false);
        //     vTaskDelay(500 / portTICK_PERIOD_MS);
        // }
        // vTaskDelay(3000 / portTICK_PERIOD_MS);

        // // Horizontal Scroll
        // ssd1306_clear_screen(&oledScreen, false);
        // ssd1306_contrast(&oledScreen, 0xff);
        // ssd1306_display_text(&oledScreen, center, "Horizontal", 10, false);
        // ssd1306_hardware_scroll(&oledScreen, SCROLL_RIGHT);
        // vTaskDelay(5000 / portTICK_PERIOD_MS);
        // ssd1306_hardware_scroll(&oledScreen, SCROLL_LEFT);
        // vTaskDelay(5000 / portTICK_PERIOD_MS);
        // ssd1306_hardware_scroll(&oledScreen, SCROLL_STOP);
        
        // // Vertical Scroll
        // ssd1306_clear_screen(&oledScreen, false);
        // ssd1306_contrast(&oledScreen, 0xff);
        // ssd1306_display_text(&oledScreen, center, "Vertical", 8, false);
        // ssd1306_hardware_scroll(&oledScreen, SCROLL_DOWN);
        // vTaskDelay(5000 / portTICK_PERIOD_MS);
        // ssd1306_hardware_scroll(&oledScreen, SCROLL_UP);
        // vTaskDelay(5000 / portTICK_PERIOD_MS);
        // ssd1306_hardware_scroll(&oledScreen, SCROLL_STOP);
        
        // // Invert
        // ssd1306_clear_screen(&oledScreen, true);
        // ssd1306_contrast(&oledScreen, 0xff);
        // ssd1306_display_text(&oledScreen, center, "  Good Bye!!", 12, true);
        // vTaskDelay(5000 / portTICK_PERIOD_MS);


        // // Fade Out
        // ssd1306_fadeout(&oledScreen);
        
        // // Fade Out
        // for(int contrast=0xff;contrast>0;contrast=contrast-0x20) {
        //     ssd1306_contrast(&oledScreen, contrast);
        //     vTaskDelay(40);
        // }

        return true;
    }

    void SSD1306Controller::DisplayText(char * text, int lineNumber, bool invert)
    {
        take_display_semaphore();
        ssd1306_display_text(&_oledScreen, lineNumber, text, strlen(text), invert);
        give_display_semaphore();
    }
    
    void SSD1306Controller::StopHorizontalScrollForLine(int lineNumber)
    {
        scrolling_lines[lineNumber][0] = NULL;
    }

    void SSD1306Controller::HorizontalScrollText(char text[], int lineNumber, SPEED speed, bool continuous_scrolling, bool invert)
    {
        if(scrolling_lines[lineNumber][0] != NULL) return;

        scroll_line_info_t * info = new scroll_line_info_t();

        info->textLength = strlen(text);
        info->pText = (char*)calloc(info->textLength+1, sizeof(char));
        sprintf(info->pText, "%s", text);
        info->pOledScreen = &_oledScreen;
        info->invertText = invert;
        info->lineNumber = lineNumber;
        info->continuous = continuous_scrolling;
        info->scrollSpeed = speed;

        xTaskCreatePinnedToCore(scroll_line_task, "scroll_line_task", 2048, info, 3, &scrolling_lines[lineNumber][0], 1);
    }

}