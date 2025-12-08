<h1 align = "center">UEDX48480021-MD80ESP32_2.1inch-Touch-Knob-Display</h1>

<p align="center" width="90%">
    <img src="image/%E5%BE%AE%E4%BF%A1%E5%9B%BE%E7%89%87_20241129110611.jpg" alt="">
</p>

## **[English](./README.md) | [中文](./README_CN.md)**

## 仓库目录介绍

```
├── Libraries（库文件）                 Arduino示例所需的库文件  
├── Schematic（原理图）                 产品的电路原理图  
├── examples（示例文件）                  示例文件，包括IDF框架和Arduino框架  
├── image（图片）                     产品或示例项目相关的图片  
├── information（信息）               产品规格，包括涉及的集成电路或外设  
├── tools（工具）                     烧录工具和图像转换工具  
└── README_CN.md                 这是您当前正在阅读的文件，简要介绍了该产品
```

## 版本迭代：
| 开发板版本 | 屏幕尺寸 | 分辨率 | 更新日期 | 更新说明 |
| :-------------------------------: | :-------------------------------: | :-------------------------------: | :-------------------------------: | :-------------------------------: |
| UEDX48480021-MD80ET | 2.1英寸 | 480*480 | 2024-07-23 | 初始版本 |

## 购买链接
| 产品                     | 系统级芯片（SOC） |  闪存   |  伪静态随机访问存储器（PSRAM）  | 链接                   |
| :----------------------: | :-------------: |:------: | :------------------------: | :------------------: |
| UEDX48480021-MD80ET   | ESP32S3R8       |  16M   | 8M（八进制串行外设接口）        | [VIEWE商城](https://viewedisplay.com/product/esp32-7-inch-800x480-rgb-ips-tft-display-touch-screen-arduino-lvgl-uart/)  |

## 目录
- [描述](#描述)
- [硬件模块](#硬件模块)
- [引脚概述](#引脚概述)
- [快速入门](#快速入门)
- [常见问题](#常见问题)
- [原理图](#原理图)
- [相关资料](#相关资料)

## 描述
UEDX48480021-MD80ESP32_2.1英寸-触摸旋钮显示屏是一款基于ESP32S3的开发板，配备2.1英寸正方形、480×480分辨率的显示屏，适用于带显示屏的微控制器项目开发。

## 硬件模块
### 1.微控制单元（MCU）
* 芯片：ESP32-S3-R8
* 伪静态随机访问存储器（PSRAM）：8M（八进制串行外设接口）
* 闪存（FLASH）：16M
* 更多详情，请访问[乐鑫ESP32-S3数据手册](https://www.espressif.com.cn/sites/default/files/documentation/esp32-s3_datasheet_en.pdf)
  
### 2. 屏幕
* 尺寸：2.1英寸IPS屏幕
* 分辨率：480x480像素
* 屏幕类型：IPS
* 驱动芯片：ST7701S
* 兼容库：ESP32_Display_Panel
* 总线通信协议：3线SPI-RGB 24位
  
### 3. 触摸
* 触摸芯片：CST826
* 总线通信协议：IIC

## 引脚概述
| IPS屏幕引脚 | ESP32S3引脚|
| :------------------: | :------------------:|
| DE         | IO17      |
| VSYNC      | IO3       |
| HSYNC      | IO46      |
| PCLK       | IO9       |
|   DATA0       |  IO10   | //B
|   DATA1       |  IO11   |
|   DATA2       |  IO12   |
|   DATA3       |  IO13   |
|   DATA4       |  IO14   |
|   DATA5       |  IO21   |  //G
|   DATA6       |  IO47   |
|   DATA7       |  IO48   |
|   DATA8       |  IO45   |
|   DATA9       |  IO38   |
|   DATA10      |  IO39   |
|   DATA11      |  IO40   |  //R
|   DATA12      |  IO41   |
|   DATA13      |  IO42   |
|   DATA14      |  IO2   |
|   DATA15      |  IO1   |
|   SPI_CS      |  IO18  |
|   SPI_SCK     |  IO13  |
|   SPI_SDA     |  IO12  |
| RST        | IO8       |
| BACKLIGHT  | IO7       |

| 触摸引脚 | ESP32S3引脚 |
| :------------------: | :------------------:|
|   SCL    | IO15       |
|   SDA    | IO16   |

| 按钮引脚 | ESP32S3引脚 |
| :------------------: | :------------------:|
|   boot    | IO0       |
|   reset   | chip-en   |

| 编码器引脚 | ESP32S3引脚 |
| :------------------: | :------------------:|
| PHA         | IO6       |
| PHB         | IO5       |

| USB/UART引脚 | ESP32S3引脚 |
| :------------------: | :------------------:|
| USB-DN         | IO19      |
| USB-DP         | IO20      |
| UART RX        | IO43      |
| UART TX        | IO44      |

## 快速入门
### 示例支持
|示例|支持的IDE及版本|描述|图片|
| ------  | ------  | ------ | ------ | 
| [ESP-IDF](./examples/ESP-IDF) | `[ESP-IDF V5.1/5.2/5.3]` | idf 驱动程序示例代码 |  |
| [SquareLinePorting](./examples/SquareLinePorting) | `[Arduino IDE][esp32_v2.0.14]` | 适用于Arduino的SquareLine移植示例 |  |


| 固件 | 描述 | 图片 |
| ------  | ------  | ------ |
| [ESP-IDF]() | 原版 |  |

### 软件框架配置
| 支持的集成开发环境 | 版本 |
| ------  | ------  |
| `[ESP-IDF]` | `[V5.1/5.2/5.3]` |
| `[Arduino IDE]` | `[esp32 >=v3.0.7]` | 
| `[Platformio IDE]` |  |

### ESP-IDF框架（[新手教程](https://github.com/VIEWESMART/VIEWE-Tutorial/blob/main/esp-idf/esp-idf%E6%96%B0%E6%89%8B%E5%85%A5%E9%97%A8%E6%95%99%E7%A8%8B.md)）
- 支持版本：v5.1/5.2/5.3
- 从代码仓库下载示例代码，可直接编译/运行。
- 仓库地址：[示例](examples/esp_idf)

### Arduino框架([新手教程](https://github.com/VIEWESMART/VIEWE-Tutorial/blob/main/Arduino%20Tutorial/Arduino%20Getting%20Started%20Tutorial.md))
1. **安装[Arduino](https://www.arduino.cc/en/software)**
- 根据你的系统类型选择安装方式。
- 新手请参考[初学者教程](https://github.com/VIEWESMART/VIEWE-Tutorial/blob/main/Arduino%20Tutorial/Arduino%20Getting%20Started%20Tutorial.md)。

2. **安装ESP32 SDK**
- 打开Arduino IDE
- 前往 `File` > `Preferences`
- 添加到 `Additional boards manager URLs`:
  ```
  https://espressif.github.io/arduino-esp32/package_esp32_index.json
  ```
  
- 导航至`Tools` > `Board` > `Boards Manager`
- 搜索 `esp32` by `Espressif Systems`
- 选择 `3.1.0` 以及以上，点击 `INSTALL` 按钮安装

3. **安装所需库**
   
  `ESP32_Display_Panel` 及其依赖项可在Arduino库管理器中获取。在线安装：

 - 在Arduino IDE中，前往 `Sketch` > `Include Library` > `Manage Libraries...`.
 - 搜索`ESP32_Display_Panel` 库和选择 `1.0.3` 及其以上, 点击 `Install` 按键安装, 系统会提示您是否安装其依赖项，请点击 `INSTALL ALL` 安装所有.
 - 安装`LVGL`库（可选），推荐版本为`8.4.0`。
 对于手动安装，您可以从[Github](https://github.com/esp-arduino-libs/ESP32_Display_Panel)或[Arduino Library](https://www.arduinolibraries.info/libraries/esp32_display_panel)下载所需版本的`.zip`文件，然后在Arduino IDE中依次点击`Sketch` > `Include Library` > `Add .ZIP Library...`，选择下载好的`.zip`文件并点击`Open`进行安装。

> [!NOTE]
> * LVGL仅在图形用户界面示例中是必需的

4. **选择并配置开发板**
- 导航至 `Tools` > `Board` > `esp32` > `ESP32S3 Dev Module`

5. **打开示例**
- 导航至 `File` > `Examples` > `ESP32_Display_Panel`
- 选择 `Arduino` > `gui` > `lvgl_v8` > `simple_port`

6. **修改代码**
- 修改`esp_panel_board_supported_conf.h`中的宏定义以启用目标板。
- 启用文件宏定义：`#define ESP_PANEL_BOARD_DEFAULT_USE_SUPPORTED       (0)` ---> `#define ESP_PANEL_BOARD_DEFAULT_USE_SUPPORTED       (1)`
- 取消相应板卡的注释：`// #define BOARD_VIEWE_UEDX48480021-MD80ET` ---> `#define BOARD_VIEWE_UEDX48480021-MD80ET`
- 以下是修改后的`esp_panel_board_supported_conf.h`文件的部分内容：

    ```c
    ...
    /**
    * @brief Flag to enable supported board configuration (0/1)
    *
    * Set to `1` to enable supported board configuration, `0` to disable
    */
    #define ESP_PANEL_BOARD_DEFAULT_USE_SUPPORTED       (1)
    ...
    // #define BOARD_VIEWE_SMARTRING
    // #define BOARD_VIEWE_UEDX24240013_MD50E
    // #define BOARD_VIEWE_UEDX24320024E_WB_A
    // #define BOARD_VIEWE_UEDX24320028E_WB_A
    // #define BOARD_VIEWE_UEDX24320035E_WB_A
    // #define BOARD_VIEWE_UEDX32480035E_WB_A
    // #define BOARD_VIEWE_UEDX46460015_MD50ET
    // #define BOARD_VIEWE_UEDX48270043E_WB_A
    // #define BOARD_VIEWE_UEDX48480021_MD80E_V2
    // #define BOARD_VIEWE_UEDX48480021_MD80E
    #define BOARD_VIEWE_UEDX48480021_MD80ET
    // #define BOARD_VIEWE_UEDX48480028_MD80ET
    // #define BOARD_VIEWE_UEDX48480040E_WB_A
    // #define BOARD_VIEWE_UEDX80480043E_WB_A
    // #define BOARD_VIEWE_UEDX80480050E_AC_A
    // #define BOARD_VIEWE_UEDX80480050E_WB_A
    // #define BOARD_VIEWE_UEDX80480050E_WB_A_2
    // #define BOARD_VIEWE_UEDX80480070E_WB_A
    ...
    ```

> [!WARNING]
> * 不要同时启用`ESP_PANEL_BOARD_DEFAULT_USE_SUPPORTED`和`ESP_PANEL_BOARD_DEFAULT_USE_CUSTOM`
> * 你不能同时启用多个板

7. 配置工具选项：
    #### ESP32-S3
    | Setting                               | Value                         |
    | :-------------------------------: | :-------------------------------: |
    | Board                                 | ESP32S3 Dev Module            |
    | Core Debug Level                | None                                |
    | USB CDC On Boot                | Disabled                             |
    | USB DFU On Boot                | Disabled                             |
    | Flash Size                           | 16MB (128Mb)                   |
    | Partition Scheme                | 16M Flash (3MB APP/9.9MB FATFS)     |
    | PSRAM                                | OPI PSRAM                      |
   
8. 选择正确的端口。
9. 点击右上角的“<kbd>[√](image/8.png)</kbd>”进行编译，若编译正确，将微控制器连接到电脑，点击右上角的“<kbd>[→](image/9.png)</kbd>”进行下载。
    
> [!NOTE]
> LVGL颜色交换设置：`SPI`和`QSPI`屏幕需要将`lv_conf.h`中的宏`LV_COLOR_16_SWAP`设置为`1`，而`RGB`屏幕则设置为`0`，如下所示：
    ```c
    /**
     * @file lv_conf.h
     * Configuration file for v8.4.0
     */
    
    /* clang-format off */
    #if 1 /*Set it to "1" to enable content*/
    
    #ifndef LV_CONF_H
    #define LV_CONF_H
    
    #include <stdint.h>
    
    /*====================
       COLOR SETTINGS
     *====================*/
    
    /*Color depth: 1 (1 byte per pixel), 8 (RGB332), 16 (RGB565), 32 (ARGB8888)*/
    #define LV_COLOR_DEPTH 16
    
    /*Swap the 2 bytes of RGB565 color. Useful if the display has an 8-bit interface (e.g. SPI)*/
    #define LV_COLOR_16_SWAP 1
    ...
    ```

### PlatformIO
1. 安装[Visual Studio Code](https://code.visualstudio.com/Download)，根据你的系统类型选择安装版本。
2. 打开Visual Studio Code软件侧边栏的“扩展”部分（或者使用“<kbd>Ctrl</kbd>+<kbd>Shift</kbd>+<kbd>X</kbd>”打开扩展），搜索“PlatformIO IDE”扩展并下载。
3. 在扩展程序安装过程中，你可以前往GitHub下载该程序。点击绿色文字的“<> Code”，即可下载主分支。
4. 扩展程序安装完成后，打开侧边栏中的资源管理器（或者使用“<kbd>Ctrl</kbd>+<kbd>Shift</kbd>+<kbd>E</kbd>”打开），点击“打开文件夹”，找到你刚刚下载的项目代码（整个文件夹），然后找到PlatformIO文件夹并点击“添加”。此时，项目文件将被添加到你的工作区。
5. 打开项目文件夹中的“platformio.ini”文件（PlatformIO会自动打开与所添加文件夹对应的“platformio.ini”文件）。在“[platformio]”部分下，取消注释并选择你想要烧录的示例程序（该程序应以“default_envs = xxx”开头）。然后点击左下角的“<kbd>[√](image/4.png)</kbd>”进行编译，若编译正确，将微控制器连接到电脑，再点击左下角的“<kbd>[→](image/5.png)</kbd>”下载程序。

### 固件下载
1. 打开项目文件“tools”，找到ESP32烧录工具并打开它。
2. 选择正确的烧录芯片和烧录方式，然后点击“确定”。如图所示，按照步骤1->2->3->4->5进行程序烧录。如果烧录不成功，按住“BOOT-0”按钮，然后重新下载并烧录。
3. 烧录项目文件根目录下的“[firmware](./firmware/)”文件，里面有固件文件版本的说明，只需选择合适的版本进行下载即可。
   
<p align="center" width="100%">
    <img src="image/10.png" alt="example">
    <img src="image/11.png" alt="example">
</p>

## 常见问题
* 问：阅读了上述教程后，我仍然不知道如何搭建编程环境。我该怎么办？
* 答：如果阅读上述教程后仍不理解如何搭建环境，您可以参考[VIEWE-FAQ]()文档中的说明进行搭建。
<br />

* 问：为什么打开Arduino IDE时会提示我更新库文件？我应该更新它们吗？
* 答：选择不更新库文件。不同版本的库文件可能不兼容，因此不建议更新库文件。
<br />

* 问：为什么我的开发板上的“Uart”接口没有串行数据输出？它是不是有缺陷，不能用了？
* 答：默认的项目配置将USB接口用作Uart0串行输出，用于调试。“Uart”接口与Uart0相连，因此如果不进行配置，它不会输出任何数据。<br />对于PlatformIO用户，请打开项目文件“platformio.ini”，将“build_flags = xxx”下的选项从“-D ARDUINO_USB_CDC_ON_BOOT=true”修改为“-D ARDUINO_USB_CDC_ON_BOOT=false”，以启用外部“Uart”接口。<br />对于Arduino用户，打开“工具”菜单，选择“USB CDC On Boot: Disabled”，以启用外部“Uart”接口。
<br />

* 问：为什么我的开发板总是无法下载程序？
* 答：请按住“BOOT”按钮，然后尝试再次下载程序。
<br />

## 原理图
<p align="center" width="100%">
    <img src="Schematic/MD80ET-V1.0%20SCH_00.png" alt="example">
</p>

## 相关资料
[产品规格](information/UEDX48480021-MD80ET%20V1.1%20SPEC.pdf))

[显示屏数据表](information/ALL-UE021WV-RB40-A009A%20V1.0%20SPEC.pdf)

[按钮](information/6x6Silent%20switch.pdf)

[编码器](information/C219783_%E6%97%8B%E8%BD%AC%E7%BC%96%E7%A0%81%E5%99%A8_EC28A1520401_%E8%A7%84%E6%A0%BC%E4%B9%A6_WJ239718.PDF)

[esp32-s3-wroom-1 数据手册(English)](information/esp32-s3-wroom-1_wroom-1u_datasheet_cn.pdf)

[esp32-s3-wroom-1 数据手册(Chinese)](information/esp32-s3-wroom-1_wroom-1u_datasheet_en.pdf)

