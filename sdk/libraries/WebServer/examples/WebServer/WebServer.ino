#include "WiFiCommon.h"
#include "WebServer.h"

WebServer ws;
uint32_t timeRef = millis();

int root_handler(void *ctx) {
    log_i("root_handler");
    WebServer::sendRedirect(ctx, "/index.html");
    return 0;
}

int index_handler(void *ctx) {
    log_i("index_handler");
    WebServer::send(ctx, "hello world");
    return 0;
}

int echo_handler(void *ctx) {
    log_i("echo_handler");
    String recvInfo =  WebServer::recvAll(ctx, 5000).c_str();
    log_i("recv: %s", recvInfo.c_str());
    WebServer::send(ctx, recvInfo);
    return 0;
}

void setup() {
    WiFi.softAPConfig("192.168.200.1", "192.168.200.1", "255.255.255.0");
    WiFi.softAP("Arduino-WiFi", "12345678");

    ws.get("/",  root_handler);
    ws.get("/index.html", index_handler);
    ws.post("/echo", echo_handler);

    if(!ws.start(80)) {
        log_e("WebServer start failed");
        while(true) {
            delay(500);
        }
    }

}

void loop() {

    delay(10);

    if(millis() - timeRef > 1000) {
        timeRef = millis();
        log_i("hello world");
    }
    
}