#include "OpusCommon.h"
#include "HardwareAudio.h"
#include "test_pcm_data.h"

#define SAMPLE_RATE 16000
#define SAMPLES_PER_FRAME 320 //20ms@16kHz
#define OPUS_BITRATE 16000

#define OPUS_DATA_LEN    (OPUS_BITRATE * SAMPLES_PER_FRAME / SAMPLE_RATE / 8)

typedef struct {
    uint16_t len;
    uint8_t data[OPUS_DATA_LEN * 2];
} opus_frame_t;

opus_frame_t opus_frame[sizeof(test_pcm_data) / (SAMPLES_PER_FRAME * 2 * 2)];
uint32_t frame_index = 0;
uint8_t buffer[SAMPLES_PER_FRAME * 2 * 2];
uint32_t refTime;

void setup() {
    logSystemInit(115200, LOG_LEVEL_INFO);

    System.d25Start();
    System.d25LogLevel(LOG_LEVEL_INFO);
    
    int version = Opus.getVersion();
    log_i("opus version: %08x", version);

    bool ret = Opus.encodeInit(1, 16000, SAMPLES_PER_FRAME, OPUS_APPLICATION_VOIP, 16000, 4, true);
    log_d("opus prepare encoder ret: %d", ret);

    uint16_t offset = 0;
    uint8_t *pcm_data = (uint8_t *)test_pcm_data + offset;
    size_t pcm_len = sizeof(test_pcm_data) - offset;
    size_t rw_len, data_len, out_len = 0;

    while(pcm_len) { 
        rw_len = pcm_len > SAMPLES_PER_FRAME * 2 * 2 ? SAMPLES_PER_FRAME * 2 * 2 : pcm_len;
        memcpy(buffer, pcm_data, rw_len);
        data_len = Audio.dualTomono(buffer, rw_len);
        log_d("start encode frame: %d, rw_len: %d", frame_index, rw_len);
        ret = Opus.encode(buffer, data_len, opus_frame[frame_index].data, sizeof(opus_frame[frame_index].data), &out_len);
        opus_frame[frame_index].len = out_len;           
        log_d("opus encode ret: %d, out_len: %d", ret, out_len);
        (void)ret;
        pcm_data += rw_len;
        pcm_len -= rw_len;
        frame_index++;
        log_d("pcm_len: %d, rw_len: %d, out_len: %d", pcm_len, rw_len, out_len);
    }

    Audio.begin(20);
    Audio.startPlay(AUDIO_MAX_LEN);
    ret = Opus.decodeInit(1, 16000, SAMPLES_PER_FRAME);
    refTime = millis();
}

void loop() {
    if (millis() - refTime > 2000) {
        log_i("start decode frame: %d", frame_index);
        refTime = millis();
        size_t out_len = 0, data_len; 
        for (uint32_t i = 0; i < frame_index; i++) {
            if(Opus.decode(opus_frame[i].data, opus_frame[i].len, buffer, sizeof(buffer), &out_len)) {
                data_len = Audio.monoTodual(buffer, out_len, sizeof(buffer));
                Audio.playSync(buffer, data_len, 20);
            }
        }
    }
}
