#include "FS.h"

uint32_t timeRef = millis();
fs::File file;

void file_read() {
    uint8_t buf[256] = {0};
    file.read(buf, sizeof(buf)-1);
    log_d("read:%s", buf);
}

void setup() {
    file = littleFS.open("/hello.txt", "r");
    file_read();
    file.close();
}

int cnt = 0;
void loop() {

    delay(10);

    if(millis() - timeRef > 1000) {
        file = littleFS.open("/hello.txt", "r");
        log_d("file.seek:%d", file.seek(0));
        file_read();
        file.close();
        timeRef = millis();
        log_i("hello world");
    }
    
}