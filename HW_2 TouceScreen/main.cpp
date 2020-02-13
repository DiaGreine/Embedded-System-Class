#include "mbed.h"
#include "stm32f413h_discovery.h"
#include "stm32f413h_discovery_ts.h"
#include "stm32f413h_discovery_lcd.h"

TS_StateTypeDef  TS_State = {0};
DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);

int main()
{
    uint16_t x1, y1;

    BSP_LCD_Init();

    /* Touchscreen initialization */
    if (BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize()) == TS_ERROR) {
        printf("BSP_TS_Init error\n");
    }

    /* Clear the LCD */
    BSP_LCD_Clear(LCD_COLOR_WHITE);

    int zonesize = BSP_LCD_GetXSize() / 3;
    int line1 = zonesize;
    int line2 = zonesize * 2;

    while (1) {
        BSP_TS_GetState(&TS_State);
        if(TS_State.touchDetected) {
            /* One or dual touch have been detected          */

            /* Get X and Y position of the first touch post calibrated */
            x1 = TS_State.touchX[0];
            y1 = TS_State.touchY[0];
        
            if (0 <= x1 <= line1) {
                led1.write(1);
            } else if (line1 <= x1 <= line2) {
                led2.write(1);
            } else if (line2 <= x1 <= BSP_LCD_GetXSize()) {
                led3.write(1);
            }


            thread_sleep_for(10);
        } else {
            led1.write(0);
            led2.write(0);
            led3.write(0);
        }
    }
}
