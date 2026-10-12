# tl_arduino_wifi_sdk Release Note

## V1.0.0

### Version

* SDK Version: tl_arduino_wifi_sdk V1.0.0
* Chip Version
  - TLSR9118(W91)
* Hardware EVK Version
  - AIOT_DK2(ML9118A): C1T301A34_V3.3
* Toolchain Version
  - TLSR9118(W91): nds32le-elf-mculib-v5

<hr style="border-bottom:2.5px solid rgb(146, 240, 161)">


### Features
* **wifi**
  * support wifi 2.4G
  * support STA and SoftAP modes
* **arduino API**
  * support official Arduino core API, including digital/analog IO, PWM, tone, UART Serial, I2C Wire, Time (delay/millis), Math, etc.
* **libraries**
  * WiFi: STA / SoftAP / Scan
  * WebServer: HTTP server and AP mode WiFi configuration (SoftAP + web page provisioning)
  * HttpClient: HTTP client
  * MQTT: MQTT over TCP / WebSocket
  * WebSocket: WebSocket client
  * Audio: audio playback / record & play
  * Opus: Opus audio codec
  * VoiceWakeup: keyword spotting and voice collection
  * InoAssistant: voice assistant
  * FS: LittleFS file system
  * Network: TCP client

### Bug Fixes
* N/A
### Refactoring
* N/A
### BREAKING CHANGES
* N/A
### Performance Improvements
* N/A


## V1.0.0

### 版本

* SDK 版本: tl_arduino_wifi_sdk V1.0.0
* 芯片版本
  - TLSR9118(W91)
* 开发板版本
  - AIOT_DK2(ML9118A): C1T301A34_V3.3

* 工具链版本
  - TLSR9118(W91): nds32le-elf-mculib-v5

<hr style="border-bottom:2.5px solid rgba(36, 190, 62, 1)">

### Features
* **wifi**
  * 支持 2.4G WiFi
  * 支持 STA 和 SoftAP 模式
* **arduino API**
  * 支持官方 Arduino 核心 API，包括数字/模拟 IO、PWM、tone、UART 串口、I2C Wire、时间（delay/millis）、Math 等
* **libraries**
  * WiFi：STA / SoftAP / Scan
  * WebServer：HTTP 服务器及 AP 模式配网（SoftAP + 网页配网）
  * HttpClient：HTTP 客户端
  * MQTT：MQTT over TCP / WebSocket
  * WebSocket：WebSocket 客户端
  * Audio：音频播放 / 录音播放
  * Opus：Opus 音频编解码
  * VoiceWakeup：语音唤醒及语音采集
  * InoAssistant：语音助手
  * FS：LittleFS 文件系统
  * Network：TCP 客户端

### Bug Fixes
* N/A
### Refactoring
* N/A
### BREAKING CHANGES
* N/A
### Performance Improvements
* N/A