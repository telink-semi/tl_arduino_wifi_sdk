#include "WiFiCommon.h"
#include "WebSocketCommon.h"
#include "StreamController.h"
#include "HardwareAudio.h"
#include "SoftwareKey.h"
#include "WiFiConfigure.h"
#include "OpusCommon.h"

#define SERVER_IP_ADDR "xxx.xxx.xxx.xxx"
#define SERVER_PORT    4567
#define KEY_CONTRL_PIN GPIO4

#define SAMPLE_RATE       16000
#define SAMPLES_PER_FRAME 320 //20ms@16kHz
#define OPUS_BITRATE      16000
#define OPUS_DATA_LEN     (OPUS_BITRATE * SAMPLES_PER_FRAME / SAMPLE_RATE / 8)

#define TAG_OPS "<OPS"
#define TAG_TXT "<TXT"
#define TAG_RST "<RST"
#define TAG_END "<END"
#define TAG_PLY "<PLY"
#define TAG_STP "<STP"

typedef enum {
  RECORD_IDLE,
  RECORD_START,
  RECORD_SND_DATA,
  RECORD_END,
}record_state_t;

typedef struct {
  record_state_t state;
  uint32_t frameCnt;
}record_context_t;

uint32_t testTime;
WebSocketClient client;

StreamController streamCtl(1024);
record_context_t recordContext;
String playText;
SoftwareKey button(KEY_CONTRL_PIN);

uint8_t pcm_buffer[SAMPLES_PER_FRAME * 2 * 2]; // 2 bytes per channel, 2 channels
uint8_t opus_enc_buffer[OPUS_DATA_LEN];
uint16_t mono_write_index = 0;
RingBuffer opus_buffer(4096);

#define MONO_WRITE_MAX (sizeof(pcm_buffer) >> 1)

static void onPLYBegin(void) {
  // log_i("onPLYBegin");
  opus_buffer.clear();
  Opus.decodeInit(1, SAMPLE_RATE, SAMPLES_PER_FRAME);
  Audio.startPlay(AUDIO_MAX_LEN);
}

static void onSTPBegin(void) {
  // log_i("onPCMEnd");
  Audio.stopPlay();
}

static size_t onOPSRecv(void *data, size_t len) {
  // log_d("onOPSRecv, len:%d, opus_buffer:%d", len, opus_buffer.freeSize());
  if(recordContext.state == RECORD_IDLE) {
    return opus_buffer.push(data, len);
  } 
  
  return len;
}

static void onTXTBegin(void) {
  // log_i("onTXTBegin");
  playText.clear();
}

static size_t onTXTRecv(void *data, size_t len) {
  // log_i("onPCMRecv");
  char buffer[256];
  char *ptr = (char *)data;
  size_t rw_len;
  size_t total_len = 0;
  while(len) {
    rw_len = (len > 255)? 255: len;
    memcpy(buffer, ptr, rw_len);
    buffer[rw_len] = '\0';
    playText += buffer;
    len -= rw_len;
    ptr += rw_len;
    total_len += rw_len;
  }
  return total_len;
}

static void onTXTEnd(void) {
  // log_i("onTXTEnd");
  log_i("%s", playText.c_str());
}

void recordHandle() {
  uint32_t recordLen;
  size_t outLen = 0;

  if(recordContext.state == RECORD_SND_DATA || recordContext.state == RECORD_END) {
    do {
      recordLen = sizeof(pcm_buffer);
      if((Audio.recordAvailable() < recordLen) || (client.availableForWrite() < (int)(recordLen >> 1))) break;
      recordLen = Audio.readRecordAsync(pcm_buffer, recordLen);
      recordLen = Audio.dualTomono(pcm_buffer, recordLen);
      if(!Opus.encode(pcm_buffer, recordLen, opus_enc_buffer, sizeof(opus_enc_buffer), &outLen)) {
        log_e("Opus.encode failed");
        break;
      }
      uint8_t packet_len = outLen;
      client.write(&packet_len, 1);
      client.write(opus_enc_buffer, outLen);
      recordContext.frameCnt += 1;
    } while(0);

    if(recordContext.state == RECORD_END) {
      recordLen = client.availableForWrite();
      if(recordLen >= TAG_LENGTH) {
        recordLen = TAG_LENGTH;
        client.write((uint8_t *)TAG_END, recordLen);
        recordContext.state = RECORD_IDLE;
        log_d("recordHandle -> total: %d", recordContext.frameCnt);
      }
    }

  } else if(recordContext.state == RECORD_START) {
    recordLen = client.availableForWrite();
    if(recordLen >= 2 * TAG_LENGTH) {
      recordLen = TAG_LENGTH;
      client.write((uint8_t *)TAG_RST, recordLen);
      client.write((uint8_t *)TAG_OPS, recordLen);
      recordContext.state = RECORD_SND_DATA;
      recordContext.frameCnt = 0;
      if(!Opus.encodeInit(1, SAMPLE_RATE, SAMPLES_PER_FRAME, OPUS_APPLICATION_RESTRICTED_LOWDELAY, OPUS_BITRATE, 4, true)) {
        log_e("Opus.encodeInit failed");
      }
    }
  }
}

void streamHandle(void *para) {
  (void)para;
  uint8_t buffer[256];
  uint32_t rwLen;
  while(client.available() > 0) {
    
    rwLen = streamCtl.freeSize();
    // log_i("client.available:<free:%d>, <available:%d>", rwLen, client.available());
    if(rwLen == 0) break;
    rwLen = (rwLen > sizeof(buffer))? sizeof(buffer): rwLen;
    rwLen = client.read(buffer, rwLen);
    if(rwLen == 0) break;

    streamCtl.input(buffer, rwLen);
  }

  streamCtl.handleLoop();
}

void playHandle() {
  uint8_t buffer[256];
  if(opus_buffer.available() > 0) {
    if(recordContext.state == RECORD_IDLE) {
      opus_buffer.copy(buffer, 1);
      size_t opus_packet_len = buffer[0];
      if((opus_buffer.available() >= (opus_packet_len + 1)) && (Audio.playFreeSize() >= sizeof(pcm_buffer))) {
        opus_buffer.pop(1);
        opus_buffer.copy(buffer, opus_packet_len);
        opus_buffer.pop(opus_packet_len);
        size_t outLen = 0;
        // log_d("opus_packet_len:%d, %d %d %d", opus_packet_len, buffer[0], buffer[1], buffer[2]);
        if(Opus.decode(buffer, opus_packet_len, pcm_buffer, sizeof(pcm_buffer), &outLen)) {
          size_t rwLen = Audio.monoTodual(pcm_buffer, outLen, sizeof(pcm_buffer));
          Audio.playAsync(pcm_buffer, rwLen);
        } else {
          log_e("Opus.decode failed");
        }
      }
    } else {
      opus_buffer.clear();
    }
  }

}

void audioLoop() {
  recordHandle();
  playHandle();
}

void keyDownProcess() {
  Audio.startRecord(AUDIO_MAX_LEN);
  Audio.stopPlay();
  recordContext.state = RECORD_START;
  recordContext.frameCnt = 0;
}

void keyUpProcess() {
  Audio.stopRecord();
  recordContext.state = RECORD_END;
}

void setup() {
  logSystemInit(115200, LOG_LEVEL_INFO);

  WiFiConfigureClass wifiConfigure;
  wifiConfigure.run(KEY_CONTRL_PIN);

  if(!Audio.begin(SAMPLES_PER_FRAME, SAMPLE_RATE)) {
    log_e("Audio begin failed");
    delay(5000);
    System.restart();
  }

  System.d25Start();
  System.d25LogLevel(LOG_LEVEL_INFO);

  streamCtl.registerProcess(TAG_PLY, onPLYBegin, NULL, NULL);
  streamCtl.registerProcess(TAG_STP, onSTPBegin, NULL, NULL);
  streamCtl.registerProcess(TAG_OPS, NULL, onOPSRecv, NULL);
  streamCtl.registerProcess(TAG_TXT, onTXTBegin, onTXTRecv, onTXTEnd);
  streamCtl.registerProcess(TAG_END, NULL, NULL, NULL);

  Task.createTask(streamHandle, NULL);

  button.setDownCallback(keyDownProcess);
  button.setUpCallback(keyUpProcess);

  client.setBufferSize(1024, 1024);
  client.addHeader("user", "arduino");
  client.addHeader("apiKey", "==telink@inoserver==");
  if(!client.connect(SERVER_IP_ADDR, SERVER_PORT, "/ino")) {
    log_e("client connect failed");
    delay(5000);
    System.restart();
  }

  testTime = millis();
}

void loop() {
  if(millis() - testTime >= 5000){
    testTime = millis();
    // log_i("Still Alive<%08d>, freeSize:%d, playAvailable:%d, frameSize:%d", testTime, streamCtl.freeSize(), Audio.playAvailable(), Audio.getFrameSize());
    log_d("[heap] freeSize:%d, lowLevel:%d", System.heapSize(), System.heapSizeLowLevel());
    // system("dmesg");
  }
  button.handleEvent();
  audioLoop();
}
