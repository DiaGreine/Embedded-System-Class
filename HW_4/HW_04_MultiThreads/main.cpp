#include "mbed.h"
#include "stm32f413h_discovery.h"
#include "stm32f413h_discovery_ts.h"
#include "stm32f413h_discovery_lcd.h"

DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);

Thread thread;
Thread thread2;
Thread thread3;

TS_StateTypeDef  TS_State = {0};

void led4_thread() 
{
    uint16_t color;
    BSP_LCD_Init();
    /* Clear the LCD */
    BSP_LCD_Clear(LCD_COLOR_RED);
    while (true) 
    {
        if(color == LCD_COLOR_RED) 
            color = LCD_COLOR_BLUE;
        else
            color = LCD_COLOR_RED;
    
        BSP_LCD_Clear(color);
        wait(2);
    }
}

void led3_thread() 
{
    while (true) 
    {
        led3 = !led3;
        wait(1.5);
    }
}

void led2_thread() 
{
    while (true) 
    {
        led2 = !led2;
        wait(1);
    }
}
 
int main()
{
    thread.start(led2_thread);
    thread2.start(led3_thread);
    thread3.start(led4_thread);
    
    while (true) 
    {
        led1 = !led1;
        wait(0.5);
    }
}