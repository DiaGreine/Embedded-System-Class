#include "mbed.h"
#include "stm32f413h_discovery.h"
#include "stm32f413h_discovery_ts.h"
#include "stm32f413h_discovery_lcd.h"
#include <string>

TS_StateTypeDef  TS_State = {0};
DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);

Timer timer;

int main()
{
    uint16_t x1, y1;
    int touchCount = 0;
    bool state = 0;

    BSP_LCD_Init();

    /* Touchscreen initialization */
    if (BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize()) == TS_ERROR) {
        printf("BSP_TS_Init error\n");
    }

    BSP_LCD_Clear(LCD_COLOR_WHITE);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetFont(&Font20);
    BSP_LCD_DisplayStringAt(0, 20, (uint8_t *)"Touch the screen", CENTER_MODE);

    /* Clear the LCD */

    int zonesize = BSP_LCD_GetXSize() / 3;

    while (1) {
        if(timer.read() > 5) {
            timer.stop();
            BSP_LCD_Clear(LCD_COLOR_BLUE);
            BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
            BSP_LCD_SetBackColor(LCD_COLOR_BLUE);
            string countText = "Count: ";
            countText += to_string(touchCount);
            BSP_LCD_DisplayStringAt(0, 60,(uint8_t *)&countText, CENTER_MODE);
            break;
        }
        
        BSP_TS_GetState(&TS_State);
        if(state) {
            // BSP_LCD_Clear(LCD_COLOR_WHITE);

            int timeSec = timer.read_ms() / 1000;
            string timeText = "Time: ";
            timeText += to_string(timeSec);

            string countText = "Count: ";
            countText += to_string(touchCount);

            BSP_LCD_DisplayStringAt(0, 20,(uint8_t *)&timeText, CENTER_MODE);
            BSP_LCD_DisplayStringAt(0, 40,(uint8_t *)&countText, CENTER_MODE);
        }

        if(TS_State.touchDetected) {
            /* One or dual touch have been detected          */
            if(!state){
                BSP_LCD_Clear(LCD_COLOR_WHITE);
                state = !state;
                timer.start();
            }
            else {
                touchCount += 1;
                thread_sleep_for(50);
            }
        } 
    }
}
