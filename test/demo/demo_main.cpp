//Audio Demo
#define DEMO_AUDIO_PLAY                    0x00
#define DEMO_AUIDO_RECORD_PLAY             0x01

//File System
#define DEMO_FILESYSTEM                    0x10

//MQTT
#define DEMO_MQTT_TCP                      0x20
#define DEMO_MQTT_WEBSOCKET                0x21
#define DEMO_MQTT_WEBSOCKET_AUDIO          0x22


//Web Server
#define DEMO_WEBSERVER                     0x30

//Web Socket
#define DEMO_WEBSOCKET                     0x40

//WiFi
#define DEMO_WIFI_SCAN                     0x50
#define DEMO_WIFI_SOFTAP                   0x51
#define DEMO_WIFI_STA                      0x52

//Wire
#define DEMO_WIRE_MASTER                   0x60
#define DEMO_WIRE_SCAN                     0x61
#define DEMO_WIRE_SLAVE                    0x62

//Opus
#define DEMO_OPUS                          0x70

//InoAssistant
#define DEMO_INOASSISTANT                  0x80
#define DEMO_INOASSISTANT_OPUS             0x81

//VoiceWakeup
#define DEMO_VOICE_COLLECTION              0x90
#define DEMO_VOICE_WAKEUP                  0x91

//HttpClient
#define DEMO_HTTP_CLIENT                   0xa0

//Other
#define DEMO_SERIAL                        0xf0

#define DEMO_TEST                          0xff

#define DEMO_SELECT                        DEMO_INOASSISTANT_OPUS//DEMO_VOICE_WAKEUP//DEMO_OPUS // DEMO_AUDIO_PLAY // DEMO_VOICE_WAKEUP

#include "Arduino.h"

//Audio
#if (DEMO_SELECT == DEMO_AUDIO_PLAY)
#include "../../libraries/Audio/examples/audio_play/audio_play.ino"
#elif (DEMO_SELECT == DEMO_AUIDO_RECORD_PLAY)
#include "../../libraries/Audio/examples/audio_record_play/audio_record_play.ino"

//File System
#elif (DEMO_SELECT == DEMO_FILESYSTEM)
#include "../../libraries/FS/examples/fs/fs.ino"

//MQTT
#elif (DEMO_SELECT == DEMO_MQTT_TCP)
#include "../../libraries/MQTT/examples/mqtt_tcp/mqtt_tcp.ino"
#elif (DEMO_SELECT == DEMO_MQTT_WEBSOCKET)
#include "../../libraries/MQTT/examples/mqtt_websocket/mqtt_websocket.ino"
#elif (DEMO_SELECT == DEMO_MQTT_WEBSOCKET_AUDIO)
#include "../../libraries/MQTT/examples/mqtt_websocket_audio/mqtt_websocket_audio.ino"

//Web Server
#elif (DEMO_SELECT == DEMO_WEBSERVER)
#include "../../libraries/WebServer/examples/wifi_configuration/wifi_configuration.ino"

//Web Socket
#elif (DEMO_SELECT == DEMO_WEBSOCKET)
#include "../../libraries/WebSocket/examples/websocket/websocket.ino"

//WiFi
#elif (DEMO_SELECT == DEMO_WIFI_SCAN)
#include "../../libraries/WiFi/examples/wifi_scan/wifi_scan.ino"
#elif (DEMO_SELECT == DEMO_WIFI_SOFTAP)
#include "../../libraries/WiFi/examples/wifi_softap/wifi_softap.ino"
#elif (DEMO_SELECT == DEMO_WIFI_STA)
#include "../../libraries/WiFi/examples/wifi_sta/wifi_sta.ino"

//Wire
#elif (DEMO_SELECT == DEMO_WIRE_MASTER)
#include "../../libraries/Wire/examples/wire_master/wire_master.ino"
#elif (DEMO_SELECT == DEMO_WIRE_SCAN)
#include "../../libraries/Wire/examples/wire_scan/wire_scan.ino"
#elif (DEMO_SELECT == DEMO_WIRE_SLAVE)
#include "../../libraries/Wire/examples/wire_slave/wire_slave.ino"

//Opus
#elif (DEMO_SELECT == DEMO_OPUS)
#include "../../libraries/Opus/examples/opus_test/opus_test.ino"

//InoAssistant
#elif (DEMO_SELECT == DEMO_INOASSISTANT)
#include "../../libraries/InoAssistant/examples/ino_assistant/ino_assistant.ino"
#elif (DEMO_SELECT == DEMO_INOASSISTANT_OPUS)
#include "../../libraries/InoAssistant/examples/ino_assistant_opus/ino_assistant_opus.ino"

//VoiceWakeup
#elif (DEMO_SELECT == DEMO_VOICE_COLLECTION)
#include "../../libraries/VoiceWakeup/examples/voice_collection/voice_collection.ino"
#elif (DEMO_SELECT == DEMO_VOICE_WAKEUP)
#include "../../libraries/VoiceWakeup/examples/voice_wakeup/voice_wakeup.ino"

//HttpClient
#elif (DEMO_SELECT == DEMO_HTTP_CLIENT)
#include "../../libraries/HttpClient/examples/http_client/http_client.ino"


//Other
#elif (DEMO_SELECT == DEMO_SERIAL)
#include "./demo_serial.cpp.ino"

#elif (DEMO_SELECT == DEMO_TEST)
#include "./demo_test.cpp.ino"

#else 
void setup() {

}


void loop() {

}


#endif
