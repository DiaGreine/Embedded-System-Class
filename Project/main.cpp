#include "mbed.h"
#include "stm32f413h_discovery.h"
#include "stm32f413h_discovery_ts.h"
#include "stm32f413h_discovery_lcd.h"
#include "TCPSocket.h"
#include "MQTTNetwork.h"
#include "MQTTmbed.h"
#include "MQTTClient.h"
#include <string>
#include <cstdlib>


// Global var
TS_StateTypeDef  TS_State = {0};
DigitalIn button(PA_0);
Timer timer;
WiFiInterface *wifi;

void clearScreen() {
    BSP_LCD_Clear(LCD_COLOR_WHITE);
}

void clearScreen(uint16_t color) {
    BSP_LCD_Clear(color);
}

void sentMessage(float reactionTime) {
    char* topic = "/ReactionTest";

    MQTTNetwork mqttNetwork(wifi);
    MQTT::Client<MQTTNetwork, Countdown> client(mqttNetwork);

    const char* hostname = "192.168.1.36";
    int port = 1883;
    printf("Connecting to %s:%d\r\n", hostname, port);
    int rc = mqttNetwork.connect(hostname, port);
    if (rc != 0)
    {
        printf("[Pub] rc from TCP connect is %d\r\n", rc);
    }
        
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 3;
    data.clientID.cstring = "mbed-sample";
    data.username.cstring = "";
    data.password.cstring = "";
    
    if ((rc = client.connect(data)) != 0)
    {
        printf("rc from MQTT connect is %d\r\n", rc);
    }
    else
    {
        printf("Client Connected.\r\n");
    }

    MQTT::Message message;
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.dup = false;

    char buf[100];

    string part1 = "{\"time\":\"";
    string part2 = "\"}";
    string msg = part1 + to_string(reactionTime) + part2;
    
    sprintf(buf, msg.c_str());
    printf("Client sent->%s \n",buf);
    message.payload = buf;
    message.payloadlen = strlen(buf);
    rc = client.publish(topic, message);
}

void reaction() {
    srand(time(NULL));
    int time = (rand() % 500) + 350;

    clearScreen(LCD_COLOR_RED);
    timer.start();

    wait_ms(time);
    clearScreen(LCD_COLOR_GREEN);
    float startTime = timer.read_ms();

    while(button.read() != 1) {}
    wait_ms(150);

    float finishTime = timer.read_ms();
    float reactionTime = finishTime - startTime;

    string buf  = to_string(reactionTime);
    string * result = &buf;

    clearScreen();
    BSP_LCD_DisplayStringAt(0, 40, (uint8_t *)"- Your Reaction time -", CENTER_MODE);
    BSP_LCD_SetFont(&Font20);
    BSP_LCD_DisplayStringAt(0, 70, (uint8_t *)result, CENTER_MODE);
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_DisplayStringAt(0, 100, (uint8_t *)"Press the button", CENTER_MODE);
    BSP_LCD_DisplayStringAt(0, 120, (uint8_t *)"to continue...", CENTER_MODE);
    sentMessage(reactionTime);

    while(button.read() != 1) {}
    wait_ms(150);
    clearScreen();
}

void title() {
    BSP_LCD_DisplayStringAt(0, 50, (uint8_t *)"Reaction Test", CENTER_MODE);
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_DisplayStringAt(0, 70, (uint8_t *)"Press the button", CENTER_MODE);
    BSP_LCD_DisplayStringAt(0, 90, (uint8_t *)"to continue...", CENTER_MODE);
    while(1) {
        if (button.read() == 1) {
            clearScreen();
            BSP_LCD_SetFont(&Font20);
            BSP_LCD_DisplayStringAt(0, 50, (uint8_t *)"Instruction", CENTER_MODE);
            BSP_LCD_SetFont(&Font16);
            BSP_LCD_DisplayStringAt(0, 80, (uint8_t *)"Press the button", CENTER_MODE);
            BSP_LCD_DisplayStringAt(0, 100, (uint8_t *)"as soon as the", CENTER_MODE);
            BSP_LCD_DisplayStringAt(0, 120, (uint8_t *)"screen turn green", CENTER_MODE);
            wait(4);

            clearScreen();
            BSP_LCD_DisplayStringAt(0, 40, (uint8_t *)"The Reaction Test", CENTER_MODE);
            BSP_LCD_DisplayStringAt(0, 60, (uint8_t *)"Will begin ...", CENTER_MODE);
            wait(1);

            clearScreen();
            BSP_LCD_DisplayStringAt(0, 80, (uint8_t *)"- 3 -", CENTER_MODE);
            wait(1);

            clearScreen();
            BSP_LCD_DisplayStringAt(0, 80, (uint8_t *)"- 2 -", CENTER_MODE);
            wait(1);

            clearScreen();
            BSP_LCD_DisplayStringAt(0, 80, (uint8_t *)"- 1 -", CENTER_MODE);
            wait(1);
            break;
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

    BSP_LCD_Clear(LCD_COLOR_WHITE);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetFont(&Font20);

    // MQTT and Wifi



    #ifdef MBED_MAJOR_VERSION
        printf("Mbed OS version %d.%d.%d\n\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);
    #endif

    wifi = WiFiInterface::get_default_instance();
    if (!wifi) 
    {
        printf("ERROR: No WiFiInterface found.\n");
        return -1;
    }

    printf("\nConnecting to %s...\n", MBED_CONF_APP_WIFI_SSID);
    int ret = wifi->connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2);
    if (ret != 0) 
    {
        printf("\nConnection error: %d\n", ret);
        return -1;
    }

    printf("Success\n\n");
    printf("MAC: %s\n", wifi->get_mac_address());
    printf("IP: %s\n", wifi->get_ip_address());
    printf("Netmask: %s\n", wifi->get_netmask());
    printf("Gateway: %s\n", wifi->get_gateway());
    printf("RSSI: %d\n\n", wifi->get_rssi());

    while (1) {
        title();
        reaction();
    }
}

