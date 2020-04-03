#include "mbed.h"
#include "TCPSocket.h"
#include "MQTTNetwork.h"
#include "MQTTmbed.h"
#include "MQTTClient.h"
#include <cstdlib>
#include <string>

WiFiInterface *wifi;
Thread thread;
int arrivedcount = 0;

void subscribeCallback(MQTT::MessageData& md)
{
    MQTT::Message &message = md.message;
    printf("[SUB] Message received: qos %d, retained %d, dup %d, packetid %d\n", message.qos, message.retained, message.dup, message.id);
    printf("[SUB] Payload %.*s\n", message.payloadlen, (char*)message.payload);
    ++arrivedcount;
}

void subscribe() {
    int count = 0;
    char* topic = "/SubTopic";

    wifi = WiFiInterface::get_default_instance();
    MQTTNetwork mqttNetwork(wifi);
    MQTT::Client<MQTTNetwork, Countdown> client(mqttNetwork);

    const char* hostname = "192.168.1.36";
    int port = 1883;
    printf("[Sub] Connecting to %s:%d\r\n", hostname, port);
    int rc = mqttNetwork.connect(hostname, port);
    if (rc != 0)
    {
        printf("[Sub] rc from TCP connect is %d\r\n", rc);
    }
        
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 3;
    data.clientID.cstring = "mbed-sub";
    data.username.cstring = "";
    data.password.cstring = "";
    
    if ((rc = client.connect(data)) != 0)
    {
        printf("[Sub] rc from MQTT connect is %d\r\n", rc);
    }
    else
    {
        printf("[Sub] Client Connected.\r\n");
    }
        

    if ((rc = client.subscribe(topic, MQTT::QOS0, subscribeCallback)) != 0)
    {
        printf("[Sub] rc from MQTT subscribe is %d\r\n", rc);
    }
    else
    {
        printf("[Sub] Client subscribed.\r\n");
    }
        

    MQTT::Message message;
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.dup = false;
    client.yield(1000); // This will also allow messages to be received. and wait to receive in milliseconds.
    printf("[Sub] Finishing with %d messages received\n", arrivedcount);
    printf("[Sub]Done\n\n");

}

int main()
{

    int count = 0;
    char* topic = "/TempHumid";

    printf("WiFi MQTT example\n");

    #ifdef MBED_MAJOR_VERSION
        printf("Mbed OS version %d.%d.%d\n\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);
    #endif

    wifi = WiFiInterface::get_default_instance();
    if (!wifi) 
    {
        printf("ERROR: No WiFiInterface found.\n");
        return -1;
    }

    printf("\n[Pub] Connecting to %s...\n", MBED_CONF_APP_WIFI_SSID);
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
    
    MQTTNetwork mqttNetwork(wifi);

    MQTT::Client<MQTTNetwork, Countdown> client(mqttNetwork);

    const char* hostname = "192.168.1.36";
    int port = 1883;
    printf("[Pub] Connecting to %s:%d\r\n", hostname, port);
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
        printf("[Pub] rc from MQTT connect is %d\r\n", rc);
    }
    else
    {
        printf("[Pub] Client Connected.\r\n");
    }

    thread.start(subscribe);
        

    MQTT::Message message;
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.dup = false;

    srand(time(NULL));
    int temp;
    int humid;

    while(count < 10) // client.isConnected()
    {
        
        // QoS 
        char buf[100];
        temp = (rand() % 100) + 1;
        humid = (rand() % 100) + 1;

        string part1 = "{\"Temp\":\"";
        string part2 = "\",\"Humid\":\"";
        string part3 = "\"}";
        string msg = part1 + to_string(temp) + part2 + to_string(humid) + part3;
        
        sprintf(buf, msg.c_str(), count);
        printf("[Pub] Client sent->%s \n",buf);
        message.payload = buf;
        message.payloadlen = strlen(buf);
        rc = client.publish(topic, message);
        count++;
        //Wait for 5 seconds.
        wait(5);
    }
    
    mqttNetwork.disconnect();

    if ((rc = client.disconnect()) != 0)
    {
        printf("[Pub] rc from disconnect was %d\r\n", rc);
        printf("[Pub] Client Disconnected.\r\n");
    }

    wifi->disconnect();

    printf("\n[Pub] Done\n");
}