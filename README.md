# tl_arduino_wifi_sdk README

* [中文版本](./README_CN.md)

## SDK introduction

tl_arduino_wifi_sdk is a software development kit for IoT applications. Based on the Telink ML9118A series modules, it provides network communication capabilities and development support for the entire process from prototype validation to product development.

The SDK integrates low-level hardware drivers, network protocols, system services, and related software components, and supports the development of Wi-Fi IoT products and projects.

![SDK Architecture](./SDK_Architecture.svg)

**Core capabilities**

| **Category** | **Capability** |
| --- | --- |
| **Wireless connectivity** | Provides Wi-Fi connectivity |
| **Hardware drivers** | Provides peripheral drivers such as Wi-Fi RF, GPIO, UART, I2C, ADC, PWM, and I2S |
| **Network communication** | Supports network protocols such as TCP/IP, HTTP/HTTPS, MQTT, and WebSocket |
| **System middleware** | Provides components such as LittleFS, the network provisioning service, the Opus codec, and TensorFlow Lite for Microcontrollers (TFLM) |
| **Development resources** | Provides various code examples |
| **System functions** | Provides system functions such as storage management |

You can use this SDK to develop a variety of wireless IoT products, covering application areas such as smart home and audio IoT.

**Typical applications**

| **Application areas** | **Typical products** |
| --- | --- |
| Wi-Fi smart home devices | Smart lighting, smart plugs, environmental sensors, smart locks, etc. |
| Audio IoT products | Recording cards, voice recorders, smart speakers, Internet radios, etc. |

**Support information**

For detailed information about supported chip models, development boards, development platforms, toolchains, and SDK versions, refer to the [Release Notes](./doc/release_note/tl_arduino_wifi_sdk_Release_Note.md).

## Documentation and resources

**Documentation**

| **Document** | **Description** |
| --- | --- |
| [Getting Started Guide](https://doc.telink-semi.cn/doc/en/software/res/sdk/arduino/tl_arduino_wifi_sdk/getting_started/tl_arduino_wifi_sdk_getting_started_en/) | Development environment setup, SDK acquisition, and quick start instructions |
| [Release Notes](./doc/release_note/tl_arduino_wifi_sdk_Release_Note.md) | Supported platforms, version notes, and detailed changes |

**Community and resources**

| **Resource** | **Description** | **Link** |
| --- | --- | --- |
| Telink Official Forum | Technical exchange and support | [Forum - Telink](https://forum.telink-semi.cn/) |
| Telink Official Website | Product center and documentation center | [Telink \| Chips for a Smarter IoT](https://www.telink-semi.com/) |
| Gitee / GitHub | SDK source repository | [Gitee](https://gitee.com/telink-semi/tl_arduino_wifi_sdk)/[GitHub](https://github.com/telink-semi/tl_arduino_wifi_sdk) |

## License

This project contains code and components licensed under the following licenses:

- Apache License 2.0
- GNU Lesser General Public License v2.1
- MIT License

For the specific license texts and applicable scope, refer to the LICENSE file in the project and the relevant component directories.
