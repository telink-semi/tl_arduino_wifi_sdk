/********************************************************************************************************
 * @file    port_audio.c
 *
 * @brief   This is the source file for Arduino SDK
 *
 * @author  Platform Group
 * @date    2026
 *
 * @par     Copyright (c) 2026, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/
#include "port_config.h"
#if BASED_ON_TELINK_WIFI_SDK

#include "port_interface.h"
#include "sdk_common.h"
#include "pins_arduino.h"
#include "tlsr_i2s.h"
#include "tlsr_audio.h"
#include "core.h"

#define DEFAULT_WORLD_LENGTH    TLSR_I2S_WL_16


enum {
    AUDIO_STATE_IDLE,
    AUDIO_STATE_BUSY,
};

typedef struct {
    int block_size;
    uint32_t period;
    void *mutex;
    queue_handle_t play_event_queue;
    queue_handle_t record_event_queue;
    uint8_t input_enable:1;
    uint8_t output_enable:1;
    uint8_t play_state:2;
    uint8_t record_state:2;
}audio_prv_handle_t, *audio_prv_handle_t_ptr;

typedef struct {
    uint64_t type:8;
    uint64_t len:32;
}audio_event_t, *audio_event_t_ptr;

struct _port_audio_context_t {
    void *owner;
    void *handle;
    uint16_t play_task_hwm;
    uint16_t record_task_hwm;
    uint8_t start_flag:1;
    uint8_t stop_play_flag:1;
    uint8_t stop_record_flag:1;

};

typedef struct _port_audio_context_t port_audio_ctx_t, *port_audio_ctx_t_ptr;


enum {
    AUDIO_EVENT_START_RECORD,
    AUDIO_EVENT_STOP_RECORD,
    AUDIO_EVENT_START_PLAY,
    AUDIO_EVENT_STOP_PLAY,
};

__attribute__((weak)) bool audio_play_task_callback(void *ctx, uint8_t *data, uint32_t len) {
    (void)ctx;
    (void)data;
    (void)len;
    return false;
}
__attribute__((weak)) bool audio_record_task_callback(void *ctx, uint8_t *data, uint32_t len) {
    (void)ctx;
    (void)data;
    (void)len;
    return false;
}
__attribute__((weak)) void audio_stop_callback(void *ctx) {
    (void)ctx;
}

static bool start_play(audio_prv_handle_t_ptr audio_handle) {
    bool ret = false;
    lock_mutex(audio_handle->mutex);
    do {
        // if(tlsr_i2s_start(TLSR_I2S_TX) != WITS_OK) break;
        tlsr_i2s_start(TLSR_I2S_TX);
        ret = true;
    } while(0);
    unlock_mutex(audio_handle->mutex);
    return ret;
}

static bool start_record(audio_prv_handle_t_ptr audio_handle) {
    bool ret = false;
    lock_mutex(audio_handle->mutex);
    do {
        // if(tlsr_i2s_start(TLSR_I2S_RX) != WITS_OK) break;
        tlsr_i2s_start(TLSR_I2S_RX);
        ret = true;
    } while(0);
    unlock_mutex(audio_handle->mutex);
    return ret;
}

static bool stop_play(audio_prv_handle_t_ptr audio_handle) {
    bool ret = false;
    lock_mutex(audio_handle->mutex);
    do {
        // if(tlsr_i2s_stop(TLSR_I2S_TX) != WITS_OK) break;
        tlsr_i2s_stop(TLSR_I2S_TX);
        ret = true;
    } while(0);
    unlock_mutex(audio_handle->mutex);
    return ret;
}

static bool stop_record(audio_prv_handle_t_ptr audio_handle) {
    bool ret = false;
    lock_mutex(audio_handle->mutex);
    do {
        // if(tlsr_i2s_stop(TLSR_I2S_RX) != WITS_OK) break;
        tlsr_i2s_stop(TLSR_I2S_RX);
        ret = true;
    } while(0);
    unlock_mutex(audio_handle->mutex);
    return ret;
}

static void play_task(void *para) {

    port_audio_ctx_t_ptr audio_ctx = (port_audio_ctx_t_ptr)para;
    if(!audio_ctx) {
        log_d("audio_play_task exit, hwm: %d",uxTaskGetStackHighWaterMark(NULL));
        vTaskDelete(NULL);
    }

    audio_prv_handle_t_ptr audio_handle = (audio_prv_handle_t_ptr)audio_ctx->handle;
    uint8_t *buffer = (uint8_t *)malloc(audio_handle->block_size);
    if(!buffer) {
        log_e("record_task -> malloc failed.");
        log_d("audio_record_task exit, hwm: %d",uxTaskGetStackHighWaterMark(NULL));
        vTaskDelete(NULL);
    }

    data_block_handle_t data = NULL;
    uint32_t targetCount = 0, playCount = 0;
    uint32_t refTime = millis();
    audio_ctx->stop_play_flag = 0;

    while(audio_ctx->start_flag) {
        if(audio_handle->play_state == AUDIO_STATE_IDLE) {
            if(queue_length(audio_handle->play_event_queue)) {
                data = queue_pop_front(audio_handle->play_event_queue);
                if(data) {
                    audio_event_t_ptr event = (audio_event_t_ptr)data_block_data(data);
                    if(event->type == AUDIO_EVENT_START_PLAY) {
                        log_d("<play_task> start play, len: %u", event->len);
                        if(start_play(audio_handle)) {

                            audio_handle->play_state = AUDIO_STATE_BUSY;
                            targetCount = event->len / audio_handle->block_size;
                            playCount = 0;

                        }else {
                            log_e("<play_task> start audio output error");
                        }
                    }
                    data_block_destroy(data);
                    continue;
                }
            } 

            yield();

        } else if(audio_handle->play_state == AUDIO_STATE_BUSY) {

            if(queue_length(audio_handle->play_event_queue)) {
                data = queue_pop_front(audio_handle->play_event_queue);
                if(data) {
                    audio_event_t_ptr event = (audio_event_t_ptr)data_block_data(data);
                    if(event->type == AUDIO_EVENT_STOP_PLAY) {
                        log_d("<play_task> stop play");
                        audio_handle->play_state = AUDIO_STATE_IDLE;
                        stop_play(audio_handle);
                    } else if(event->type == AUDIO_EVENT_START_PLAY) {
                        targetCount = event->len / audio_handle->block_size;
                        playCount = 0;
                    }
                    data_block_destroy(data);
                    continue;
                }
            }

            if((millis() - refTime < 0) || !audio_play_task_callback(audio_ctx, buffer, audio_handle->block_size)) {
                yield();
                continue;
            }
            refTime = millis();

            if(tlsr_i2s_write_block(buffer, audio_handle->block_size) != WITS_OK) {
                log_e("<audio><play_task> write block error.");
            }

            playCount++;
            // log_d("<play_task> playCount:%d, targetCount:%d", playCount, targetCount);
            if(playCount >= targetCount) {
                audio_handle->play_state = AUDIO_STATE_IDLE;
                stop_play(audio_handle);
            }
        }
    }  

    free(buffer);
    audio_ctx->stop_play_flag = 1;
    audio_ctx->play_task_hwm = uxTaskGetStackHighWaterMark(NULL);
    log_d("audio_play_task exit, hwm: %d", audio_ctx->play_task_hwm);

    vTaskDelete(NULL);
}

static void record_task(void *para) {
    port_audio_ctx_t_ptr audio_ctx = (port_audio_ctx_t_ptr)para;
    if(!audio_ctx) {
        log_d("audio_record_task exit, hwm: %d",uxTaskGetStackHighWaterMark(NULL));
        vTaskDelete(NULL);
    }

    audio_prv_handle_t_ptr audio_handle = (audio_prv_handle_t_ptr)audio_ctx->handle;
    uint8_t *buffer = (uint8_t *)malloc(audio_handle->block_size);
    if(!buffer) {
        log_e("record_task -> malloc failed.");
        log_d("audio_record_task exit, hwm: %d",uxTaskGetStackHighWaterMark(NULL));
        vTaskDelete(NULL);
    }

    audio_ctx->stop_record_flag = 0;
    data_block_handle_t data = NULL;
    size_t buffer_size;
    uint32_t targetCount = 0, recordCount = 0;
    static uint32_t refTime = 0;
    int ret = 0;

    while (audio_ctx->start_flag) {
        if(millis() - refTime > 5000) {
            refTime = millis();
            // log_d("record_task alive, state:%d, recordCount:%d, targetCount:%d, error:%d", audio_handle->record_state, recordCount, targetCount, ret);
        }
        if(audio_handle->record_state == AUDIO_STATE_IDLE) {
            if(queue_length(audio_handle->record_event_queue)) {
                data = queue_pop_front(audio_handle->record_event_queue);
                if(data) {
                    audio_event_t_ptr event = (audio_event_t_ptr)data_block_data(data);
                    if(event->type == AUDIO_EVENT_START_RECORD) {
                        log_d("<record_task> start record, len:%u", event->len);
                        if(start_record(audio_handle)) {
                            audio_handle->record_state = AUDIO_STATE_BUSY;
                            targetCount = event->len / audio_handle->block_size;
                            recordCount = 0;
                        } else {
                            log_e("<play_task> start audio input error");
                        }
                    }
                    data_block_destroy(data);
                    continue;
                }
            }

            yield();

        } else if(audio_handle->record_state == AUDIO_STATE_BUSY) {

            if(queue_length(audio_handle->record_event_queue)) {
                data = queue_pop_front(audio_handle->record_event_queue);
                if(data) {
                    audio_event_t_ptr event = (audio_event_t_ptr)data_block_data(data);
                    if(event->type == AUDIO_EVENT_STOP_RECORD) {
                        log_d("<record_task> stop record");
                        audio_handle->record_state = AUDIO_STATE_IDLE;
                        stop_record(audio_handle);
                    } else if(event->type == AUDIO_EVENT_START_RECORD) {
                        targetCount = event->len / audio_handle->block_size;
                        recordCount = 0;
                    }
                    data_block_destroy(data);
                    continue;
                }
            }

            buffer_size = audio_handle->block_size;

            if((ret = tlsr_i2s_read_block(buffer, &buffer_size)) == WITS_OK) {
                while(!audio_record_task_callback(audio_ctx, buffer, buffer_size)) {
                    yield();
                    if(!audio_ctx->start_flag) break;
                }
            } else {
                yield();
                continue;
            }

            recordCount++;
            // log_d("<record_task> recordCount:%d, targetCount:%d", recordCount, targetCount);
            if(recordCount >= targetCount) {
                audio_handle->record_state = AUDIO_STATE_IDLE;
                stop_record(audio_handle);
            }

            yield();

        }
    }

    free(buffer);
    audio_ctx->stop_record_flag = 1;
    if(audio_ctx->start_flag) {
        audio_stop_callback(audio_ctx);
    }

    audio_ctx->record_task_hwm = uxTaskGetStackHighWaterMark(NULL);
    log_d("audio_play_task exit, hwm: %d", audio_ctx->record_task_hwm);

    vTaskDelete(NULL);
}


void *port_audio_create_context(void *owner) {
    port_audio_ctx_t_ptr audio_ctx = (port_audio_ctx_t_ptr)malloc(sizeof(port_audio_ctx_t));
    if(audio_ctx) {
        memset(audio_ctx, 0, sizeof(port_audio_ctx_t));
        audio_ctx->owner = owner;
    }
    return audio_ctx;
}

void port_audio_destroy_context(void *ctx) {
    if(ctx) {
        free(ctx);
    }
}

bool port_audio_set_configuration(void *ctx, port_audio_configuration_t *config) {
    if(!ctx || !config) return false;
    port_audio_ctx_t_ptr audio_ctx = (port_audio_ctx_t_ptr)ctx;
    if(config->owner_valid) {
        audio_ctx->owner = config->owner;
    }
    return true;
}

bool port_audio_get_configuration(void *ctx, port_audio_configuration_t *config) {
    if(!ctx || !config) return false;
    port_audio_ctx_t_ptr audio_ctx = (port_audio_ctx_t_ptr)ctx;
    config->owner_valid = 1;
    config->owner = audio_ctx->owner;
    return true;
}


bool port_audio_begin(void *ctx, uint32_t samples, uint32_t sample_rate, bool input_enable, bool output_enable, float volume) {
    bool ret = false;
    audio_prv_handle_t_ptr audio_handle = NULL;
    struct tlsr_i2s_cfg i2s_cfg = {0};
    struct tlsr_audio_cfg audio_cfg = {0};
    port_audio_ctx_t_ptr audio_ctx = (port_audio_ctx_t_ptr)ctx;
    bool i2s_init_flag = false;
    bool audio_init_flag = false;
    do {
        if(!ctx) break;
        audio_handle = (audio_prv_handle_t *)malloc(sizeof(audio_prv_handle_t));
        if(!audio_handle) break;
        memset(audio_handle, 0, sizeof(audio_prv_handle_t));
        audio_handle->record_event_queue = queue_create(2);
        if(!audio_handle->record_event_queue) break;
        audio_handle->play_event_queue = queue_create(2);
        if(!audio_handle->play_event_queue) break;
        audio_handle->mutex = create_mutex();
        if(!audio_handle->mutex) break;

        audio_handle->play_state = AUDIO_STATE_IDLE;
        audio_handle->record_state = AUDIO_STATE_IDLE;
        audio_ctx->handle = audio_handle;
        
        if(tlsr_i2s_init() != WITS_OK) break;
        // log_d("i2s_init");
        i2s_init_flag = true;
        i2s_cfg.dir = TLSR_I2S_TX;
        i2s_cfg.word_length = DEFAULT_WORLD_LENGTH;
        i2s_cfg.format = TLSR_I2S_FMT_I2S;
        i2s_cfg.role = TLSR_I2S_ROLE_MASTER;
        i2s_cfg.fs = sample_rate;
        i2s_cfg.duration_per_block = (samples * 1000) / i2s_cfg.fs;
        audio_handle->period = i2s_cfg.duration_per_block;
        i2s_cfg.number_of_blocks = 5;
        i2s_cfg.timeout = 10000;
        if(tlsr_i2s_configure(&i2s_cfg) != WITS_OK) break;
        i2s_cfg.dir = TLSR_I2S_RX;
        if(tlsr_i2s_configure(&i2s_cfg) != WITS_OK) break;

        if(tlsr_audio_init() != WITS_OK) break;
        audio_init_flag = true;
        audio_handle->block_size = tlsr_i2s_get_block_buffer_size(&i2s_cfg);

        audio_cfg.mclk_freq = 12000000;
        audio_cfg.word_length = DEFAULT_WORLD_LENGTH;
        audio_cfg.format = TLSR_I2S_FMT_I2S;
        audio_cfg.role = TLSR_I2S_ROLE_SLAVE;
        audio_cfg.fs = sample_rate;
        if(tlsr_audio_configure(&audio_cfg) != WITS_OK) break;
        log_d("audio configure: format<%d> role<%d> fs<%d> blockSize<%d>", audio_cfg.format, audio_cfg.role, audio_cfg.fs, audio_handle->block_size);

        //start codec
        if(input_enable) {
            audio_handle->input_enable = true;
            if(tlsr_audio_start(TLSR_AUDIO_INPUT) != WITS_OK) break;
            if(tlsr_i2s_start(TLSR_I2S_RX) != WITS_OK) break;
            // if(tlsr_i2s_stop(TLSR_I2S_RX) != WITS_OK) break;
             if(tlsr_i2s_stop(TLSR_I2S_RX) != WITS_OK) {
                log_e("i2s_stop(TLSR_I2S_RX) failed");
             }
            
        }
        
        if(output_enable) {
            audio_handle->output_enable = true;
            if(tlsr_audio_start(TLSR_AUDIO_OUTPUT) != WITS_OK) break;
            if(tlsr_i2s_start(TLSR_I2S_TX) != WITS_OK) break;
            // if(tlsr_i2s_stop(TLSR_I2S_TX) != WITS_OK) break;
             if(tlsr_i2s_stop(TLSR_I2S_TX) != WITS_OK) {
                log_e("i2s_stop(TLSR_I2S_TX) failed");
             }
            
        }

        // if(tlsr_audio_set_volume(volume) != WITS_OK) break;

        audio_ctx->start_flag = 1;
        audio_ctx->stop_play_flag = 0;
        audio_ctx->stop_record_flag = 0;
        TaskHandle_t handle;
        if(audio_handle->output_enable) {
            if(xTaskCreate(play_task, "audio_play_task", 512 + 128, audio_ctx, APP_TASK_PRIORITY, &handle) != pdPASS) break;
            log_d("audio_play_task create, handle:%p", handle);
        }

        if(audio_handle->input_enable) {
            if(xTaskCreate(record_task, "audio_record_task", 512 + 128, audio_ctx, APP_TASK_PRIORITY, &handle) != pdPASS) break;
            log_d("audio_record_task create, handle:%p", handle);
        }

        ret = true;
    } while (0);

    if(!ret) {

        audio_ctx->start_flag = 0;
        if(i2s_init_flag) {
            tlsr_i2s_stop(TLSR_I2S_BOTH);
            tlsr_i2s_deinit();
        }

        if(audio_init_flag) {
            tlsr_audio_stop(TLSR_AUDIO_INPUT);
            tlsr_audio_stop(TLSR_AUDIO_OUTPUT);
            tlsr_audio_deinit();
        }

        if(audio_handle) {
            if(audio_handle->record_event_queue) {
                queue_destroy(audio_handle->record_event_queue);
            }
            if(audio_handle->play_event_queue) {
                queue_destroy(audio_handle->play_event_queue);
            }
            if(audio_handle->mutex) {
                destroy_mutex(audio_handle->mutex);
            }
            free(audio_handle);
            audio_handle = NULL;
        }

        if(audio_ctx) {
            audio_ctx->handle = NULL;
        }
    }

    return ret;
}

bool port_audio_stop(void *ctx) {
    bool ret = false;
    port_audio_ctx_t_ptr audio_ctx = (port_audio_ctx_t_ptr)ctx;
    do {
        if(!audio_ctx) break;
        audio_prv_handle_t_ptr audio_handle = (audio_prv_handle_t_ptr)audio_ctx->handle;
        if(!audio_handle) break;
        audio_ctx->start_flag = 0;

        if(audio_handle->input_enable) {
            stop_record(audio_handle);
            tlsr_audio_stop(TLSR_AUDIO_INPUT);
            while(!audio_ctx->stop_record_flag) {
                yield();
            }
        }
        if(audio_handle->output_enable) {
            stop_play(audio_handle);
            tlsr_audio_stop(TLSR_AUDIO_OUTPUT);
            while(!audio_ctx->stop_play_flag) {
                yield();
            }
        }

        tlsr_i2s_deinit();
        tlsr_audio_deinit();
        queue_destroy(audio_handle->record_event_queue);
        queue_destroy(audio_handle->play_event_queue);
        destroy_mutex(audio_handle->mutex);
        free(audio_handle);
        audio_ctx->handle = NULL;

        ret = true;

    } while(0);

    return ret;
}

bool port_audio_is_open(void *ctx) {
    if(!ctx) return false;
    port_audio_ctx_t_ptr audio_ctx = (port_audio_ctx_t_ptr)ctx;
    return audio_ctx->handle != NULL;
}

bool port_audio_start_record(void *ctx, uint32_t len) {
    if(!ctx) return false;
    port_audio_ctx_t_ptr audio_ctx = (port_audio_ctx_t_ptr)ctx;
    if(!audio_ctx->handle) return false;
    audio_prv_handle_t_ptr audio_handle = (audio_prv_handle_t_ptr)audio_ctx->handle;
    if(!audio_handle->record_event_queue) return false;

    audio_event_t event;
    event.type = AUDIO_EVENT_START_RECORD;
    event.len = len;

    return queue_push_back_data(audio_handle->record_event_queue, &event, sizeof(audio_event_t));
}

bool port_audio_play_busy(void *ctx) {
    if(!ctx) return false;
    port_audio_ctx_t_ptr audio_ctx = (port_audio_ctx_t_ptr)ctx;
    if(!audio_ctx->handle) return false;
    audio_prv_handle_t_ptr audio_handle = (audio_prv_handle_t_ptr)audio_ctx->handle;
    return audio_handle->play_state == AUDIO_STATE_BUSY;
}

bool port_audio_record_busy(void *ctx) {
    if(!ctx) return false;
    port_audio_ctx_t_ptr audio_ctx = (port_audio_ctx_t_ptr)ctx;
    if(!audio_ctx->handle) return false;
    audio_prv_handle_t_ptr audio_handle = (audio_prv_handle_t_ptr)audio_ctx->handle;
    return audio_handle->record_state == AUDIO_STATE_BUSY;
}

bool port_audio_stop_record(void *ctx) {
    if(!ctx) return false;
    port_audio_ctx_t_ptr audio_ctx = (port_audio_ctx_t_ptr)ctx;
    if(!audio_ctx->handle) return false;
    audio_prv_handle_t_ptr audio_handle = (audio_prv_handle_t_ptr)audio_ctx->handle;
    if(!audio_handle->record_event_queue) return false;
    if(audio_handle->record_state != AUDIO_STATE_BUSY) return true;

    audio_event_t event;
    event.type = AUDIO_EVENT_STOP_RECORD;

    return queue_push_back_data(audio_handle->record_event_queue, &event, sizeof(audio_event_t));
}

bool port_audio_start_play(void *ctx, uint32_t len) {
    if(!ctx) return false;
    port_audio_ctx_t_ptr audio_ctx = (port_audio_ctx_t_ptr)ctx;
    if(!audio_ctx->handle) return false;
    audio_prv_handle_t_ptr audio_handle = (audio_prv_handle_t_ptr)audio_ctx->handle;
    if(!audio_handle->play_event_queue) return false;

    audio_event_t event;
    event.type = AUDIO_EVENT_START_PLAY;
    event.len = len;

    return queue_push_back_data(audio_handle->play_event_queue, &event, sizeof(audio_event_t));
}

bool port_audio_stop_play(void *ctx) {
    if(!ctx) return false;
    port_audio_ctx_t_ptr audio_ctx = (port_audio_ctx_t_ptr)ctx;
    if(!audio_ctx->handle) return false;
    audio_prv_handle_t_ptr audio_handle = (audio_prv_handle_t_ptr)audio_ctx->handle;
    if(!audio_handle->play_event_queue) return false;

    if(audio_handle->play_state != AUDIO_STATE_BUSY) return true;

    audio_event_t event;
    event.type = AUDIO_EVENT_STOP_PLAY;

    return queue_push_back_data(audio_handle->play_event_queue, &event, sizeof(audio_event_t));
}

uint32_t port_audio_get_buffer_size(uint32_t samples, uint32_t sample_rate) {
    struct tlsr_i2s_cfg i2s_cfg = {0};
    i2s_cfg.word_length = DEFAULT_WORLD_LENGTH;
    i2s_cfg.fs = sample_rate;
    i2s_cfg.duration_per_block = (samples * 1000) / i2s_cfg.fs;
    return tlsr_i2s_get_block_buffer_size(&i2s_cfg);
}

uint32_t port_audio_get_frame_size(void *ctx) {
    if(!port_audio_is_open(ctx)) return 0;
    port_audio_ctx_t_ptr audio_ctx = (port_audio_ctx_t_ptr)ctx;
    audio_prv_handle_t_ptr audio_handle = (audio_prv_handle_t_ptr)audio_ctx->handle;
    if(audio_handle->block_size < 0) audio_handle->block_size = 0;
    return audio_handle->block_size;
}

bool port_audio_prepare_vad(uint16_t frame_size) {
    return ipc_driver_d25_prepareVad(frame_size);
}

bool port_audio_do_vad(uint8_t *data, uint32_t len, bool rms_enable, bool zcr_enable, uint16_t rms_threshold, uint16_t zcr_threshold_min, uint16_t zcr_threshold_max) {
    return ipc_driver_d25_doVad(data, len, rms_enable, zcr_enable, rms_threshold, zcr_threshold_min, zcr_threshold_max);
}

#endif