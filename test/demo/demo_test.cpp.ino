#include <HardwareAudio.h>
#include <vector>
std::vector<std::pair<const char*, const char*>> test_pcm;

uint32_t refTime;

void setup() {
  logSystemInit(115200, LOG_LEVEL_DEBUG);
  refTime = millis();
  test_pcm.reserve(5);
  log_d("test_pcm:%p", test_pcm.data());
  test_pcm.push_back(std::make_pair("Content-Type", "application/json"));
  test_pcm.push_back(std::make_pair("User-Agent", "Arduino/1.0"));
  test_pcm.push_back(std::make_pair("Host", "example.com"));
  test_pcm.push_back(std::make_pair("Connection", "keep-alive"));
  unsigned char *test = (unsigned char *)malloc(10);
  log_d("test:%p", test);

}


void loop() {

  if(millis() - refTime > 2000) {
    log_d("hello world");
    for(size_t i = 0; i < test_pcm.size(); i++) {
      log_d("[%d] key = %s, value = %s", i, test_pcm[i].first, test_pcm[i].second);
    }
    
    refTime = millis();
  }

}

