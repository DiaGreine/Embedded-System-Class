#include "mbed.h"
#include "stm32f413h_discovery.h"
#include "stm32f413h_discovery_ts.h"
#include "stm32f413h_discovery_lcd.h"
#include <string>


Mutex stdio_mutex;
Thread t2;
Thread t3;
TS_StateTypeDef  TS_State = {0};
    
void notify(uint16_t color,int num) 
{
    stdio_mutex.lock();
    BSP_LCD_SetTextColor(color);

    string text;
    text = to_string(num);
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    BSP_LCD_DisplayStringAt(0, 50, (uint8_t *)&text, CENTER_MODE);
    stdio_mutex.unlock();
    wait(1);
}

void test_thread(void const *args)
 {
    
    int val = 0;
    uint16_t color;
    if((const char*)args == "GREEN") {
        color = LCD_COLOR_GREEN;
    } else if ((const char*)args == "RED") {
        color = LCD_COLOR_RED;
    } else if ((const char*)args == "BLUE") {
        color = LCD_COLOR_BLUE;
    }


    while (true) 
    {
        val = rand() % 100;
        BSP_TS_GetState(&TS_State);
        if(TS_State.touchDetected) {
            notify(color, val);   
            wait(1);
        }
        
    }
}

int main() 
{
    BSP_LCD_Init();
    /* Touchscreen initialization */
    if (BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize()) == TS_ERROR) {
        printf("BSP_TS_Init error\n");
    }

    /* Clear the LCD */
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    BSP_LCD_SetFont(&Font24);

    t2.start(callback(test_thread, (void *)"BLUE"));
    t3.start(callback(test_thread, (void *)"RED"));

    test_thread((void *) "GREEN");
}