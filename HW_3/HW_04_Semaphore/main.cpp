#include "mbed.h"
#include "stm32f413h_discovery.h"
#include "stm32f413h_discovery_ts.h"
#include "stm32f413h_discovery_lcd.h"
#include <string>

Semaphore one_slot(2);
Thread t2;
Thread t3;
TS_StateTypeDef  TS_State = {0};

void test_thread(void const *args) 
{
    int val = 0;
    int pos = 0;
    uint16_t color;
    if((const char*)args == "GREEN") {
        color = LCD_COLOR_GREEN;
        pos = 25;
    } else if ((const char*)args == "RED") {
        color = LCD_COLOR_RED;
        pos = 50;
    } else if ((const char*)args == "BLUE") {
        color = LCD_COLOR_BLUE;
        pos = 75;
    }

    while (true) 
    {
        val = rand() % 100;
        one_slot.wait();
        string text;
        text = to_string(val);
        BSP_LCD_SetTextColor(color);
        BSP_LCD_DisplayStringAt(0, pos, (uint8_t *)&text, CENTER_MODE);
        wait(1);
        one_slot.release();
    }
}

int main (void) 
{
    BSP_LCD_Init();
    /* Touchscreen initialization */
    if (BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize()) == TS_ERROR) {
        printf("BSP_TS_Init error\n");
    }

    /* Clear the LCD */
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    BSP_LCD_SetFont(&Font24);

    t2.start(callback(test_thread, (void *)"RED"));
    t3.start(callback(test_thread, (void *)"BLUE"));

    test_thread((void *) "GREEN");
}

