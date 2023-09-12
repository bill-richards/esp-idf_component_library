#ifndef __SSD1306_CONTROLLER_H__
#define __SSD1306_CONTROLLER_H__

#include <gsdc_ssd1306.h>
#include <font8x8_basic.h>


namespace gsdc_oled { 

        typedef enum {
            BANNER_1 = 0,
            BANNER_2,
            TOP,
            CENTER,
            BOTTOM = 6
        } DISPLAY_POSITION;

        typedef enum {
            SLOW = 20,
            MEDIUM = 10,
            FAST = 7,
            FASTEST = 5
        } SPEED;

        const int16_t DEFAULT_RESET_PIN = 15;

        class SSD1306Controller {
        public:
         static const int DISPLAY_WIDTH{17};
        private:
            SSD1306_t _oledScreen;
            char lineChar[20];
            bool _invertText = false;
            
        public:
            /**
             * @brief Displays a text string
             * @param text (char *) the text to display
             * @param lineNumber (int) the "page" upon which to display the text.
             * @param invert (bool -defaults to false) whether or not to invert the text
             */
            void DisplayText(char * text, int lineNumber, bool invert = false);
            /**
             * @brief Show the "up time" on the top two lines of the display
             */
            void DisplayUpTime(void);
            void HorizontalScrollText(char text[], int lineNumber, SPEED speed, bool continuous_scrolling = false, bool invert = false);
            bool Initialize(bool flipDisplay, int16_t data_pin, int16_t clock_pin, int16_t reset_pin = DEFAULT_RESET_PIN);
            void StopHorizontalScrollForLine(int lineNumber);
        };

} // namespace gsdc_oled

#endif // __SSD1306_CONTROLLER_H__