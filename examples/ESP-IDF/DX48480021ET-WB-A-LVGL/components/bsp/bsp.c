/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "driver/i2c.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "lvgl.h"
//#include "iot_knob.h"

#include "bsp/esp-bsp.h"
#include "bsp_err_check.h"
#include "bsp_sub_board.h"
#include "esp_lvgl_port.h"
#include "sdkconfig.h"
#include "esp_lcd_touch_cst816s.h"
static const char *TAG = "bsp";

/* LCD IO and panel */
static esp_lcd_panel_handle_t lcd_panel = NULL;

/* LVGL display and touch */
static lv_display_t *lvgl_disp = NULL;
static lv_indev_t *lvgl_encoder_indev = NULL;

/**********************************************************************************************************
 *
 * Display Function
 *
 **********************************************************************************************************/
static bsp_lcd_trans_done_cb_t trans_done = NULL;
#if CONFIG_BSP_LCD_RGB_REFRESH_TASK_ENABLE
static TaskHandle_t lcd_task_handle = NULL;
#endif

static esp_err_t lcd_config(void);

/**************************************************************************************************
 *
 * LCD Panel Function
 *
 **************************************************************************************************/
IRAM_ATTR static bool on_vsync_event(esp_lcd_panel_handle_t panel, esp_lcd_rgb_panel_event_data_t *edata, void *user_ctx)
{
    BaseType_t need_yield = pdFALSE;
#if CONFIG_BSP_LCD_RGB_REFRESH_MANUALLY
    xTaskNotifyFromISR(lcd_task_handle, ULONG_MAX, eNoAction, &need_yield);
#endif
    if (trans_done) {
        if (trans_done(panel)) {
            need_yield = pdTRUE;
        }
    }

    return (need_yield == pdTRUE);
}

#if CONFIG_BSP_LCD_RGB_REFRESH_MANUALLY
static void lcd_task(void *arg)
{
    ESP_LOGI(TAG, "Starting LCD refresh task");

    TickType_t tick;
    for (;;) {
        esp_lcd_rgb_panel_refresh((esp_lcd_panel_handle_t)arg);
        tick = xTaskGetTickCount();
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        vTaskDelayUntil(&tick, pdMS_TO_TICKS(CONFIG_BSP_LCD_RGB_REFRESH_TASK_PERIOD));
    }
}
#endif

esp_err_t bsp_lcd_register_trans_done_callback(bsp_lcd_trans_done_cb_t callback)
{
#if CONFIG_LCD_RGB_ISR_IRAM_SAFE
    if (callback) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(callback), ESP_ERR_INVALID_ARG, TAG, "Callback not in IRAM");
    }
#endif
    trans_done = callback;

    return ESP_OK;
}

/**************************************************************************************************
 *
 * LCD Configuration Function
 *
 **************************************************************************************************/
#define Delay(t)        vTaskDelay(pdMS_TO_TICKS(t))
#define udelay(t)       esp_rom_delay_us(t)
#define CS(n)           BSP_ERROR_CHECK_RETURN_ERR(gpio_set_level(BSP_LCD_SPI_CS, n))
#define SCK(n)          BSP_ERROR_CHECK_RETURN_ERR(gpio_set_level(BSP_LCD_SPI_SCK, n))
#define SDO(n)          BSP_ERROR_CHECK_RETURN_ERR(gpio_set_level(BSP_LCD_SPI_SDO, n))

/**
 * @brief Simulate SPI to write data using io expander
 *
 * @param data: Data to write
 *
 * @return
 *      - ESP_OK: Success, otherwise returns ESP_ERR_xxx
 */
static esp_err_t spi_write(uint16_t data)
{
    for (uint8_t n = 0; n < 9; n++) {
        if (data & 0x0100) {
            SDO(1);
        } else {
            SDO(0);
        }
        data = data << 1;

        SCK(0);
        udelay(10);
        SCK(1);
        udelay(10);
    }

    return ESP_OK;
}

/**
 * @brief Simulate SPI to write LCD command using io expander
 *
 * @param data: LCD command to write
 *
 * @return
 *      - ESP_OK: Success, otherwise returns ESP_ERR_xxx
 */
static esp_err_t spi_write_cmd(uint16_t data)
{
    CS(0);
    udelay(10);

    spi_write((data & 0x00FF));

    udelay(10);
    CS(1);
    SCK(1);
    SDO(1);
    udelay(10);

    return ESP_OK;
}

/**
 * @brief Simulate SPI to write LCD data using io expander
 *
 * @param data: LCD data to write
 *
 * @return
 *      - ESP_OK: Success, otherwise returns ESP_ERR_xxx
 */
static esp_err_t spi_write_data(uint16_t data)
{
    CS(0);
    udelay(10);

    data &= 0x00FF;
    data |= 0x0100;
    spi_write(data);

    udelay(10);
    CS(1);
    SCK(1);
    SDO(1);
    udelay(10);

    return ESP_OK;
}

/**
 * @brief LCD configuration data structure type
 *
 */
typedef struct {
    uint8_t cmd;            // LCD command
    uint8_t data[52];       // LCD data
    uint8_t data_bytes;     // Length of data in above data array; 0xFF = end of cmds.
} lcd_config_data_t;

const static lcd_config_data_t LCD_CONFIG_CMD[] = {
   
  
   

    #if VIEWE_DX48480021E_WB_A_with_touch
    //2.1带触摸
    {0xFF, {0x77,0x01,0x00,0x00,0x13}, 5},
    {0xEF, {0x08}, 1},
    {0xFF, {0x77,0x01,0x00,0x00,0x10}, 5},


    {0xC0, {0x3B,0x00}, 2},
    {0xC1, {0x0B,0x02}, 2},
    {0xC2, {0x07,0x02}, 2},

    {0xC7, {0x00}, 1},//0x04这个改成00 修正屏幕文字显示
    {0xCC, {0x10}, 1},
    {0xCD, {0x08}, 1},


    {0xB0, {0x00,0x11,0x16,0x0E,0x11,0x06,0x05,0x09,0x08,0x21,0x06,0x13,0x10,0x29,0x31,0x18}, 16},
    {0xB1, {0x00,0x11,0x16,0x0E,0x11,0x07,0x05,0x09,0x09,0x21,0x05,0x13,0x11,0x2A,0x31,0x18}, 16},
    {0xFF, {0x77,0x01,0x00,0x00,0x11}, 5},

    {0xB0, {0x6D}, 1},
    {0xB1, {0x37}, 1},
    {0xB2, {0x8B}, 1},
    {0xB3, {0x80}, 1},
    {0xB5, {0x43}, 1},
    {0xB7, {0x85}, 1},
    {0xB8, {0x20}, 1},


    {0xC0, {0x09}, 1},
    {0xC1, {0x78}, 1},
    {0xC2, {0x78}, 1},
    {0xD0, {0x88}, 1},


    {0xE0, {0x00,0x00,0x02}, 3},
    {0xE1, {0x03,0xA0,0x00,0x00,0x04,0xA0,0x00,0x00,0x00,0x20,0x20}, 11},
    {0xE2, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, 13},
    {0xE3, {0x00,0x00,0x11,0x00}, 4},
    {0xE4, {0x22,0x00}, 2},
    {0xE5, {0x05,0xEC,0xF6,0xCA,0x07,0xEE,0xF6,0xCA,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, 16},
    {0xE6, {0x00,0x00,0x11,0x00}, 4},
    {0xE7, {0x22,0x00}, 2},
    {0xE8, {0x06,0xED,0xF6,0xCA,0x08,0xEF,0xF6,0xCA,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, 16},
    {0xE9, {0x36,0x00}, 2},

    {0xEB, {0x00,0x00,0x40,0x40,0x00,0x00,0x00}, 7},//
    {0xED, {0xFF,0xFF,0xFF,0xBA,0x0A,0xFF,0x45,0xFF,0xFF,0x54,0xFF,0xA0,0xAB,0xFF,0xFF,0xFF}, 16},
    {0xEF, {0x08,0x08,0x08,0x45,0x3F,0x54}, 6},
    {0xFF, {0x77,0x01,0x00,0x00,0x13}, 5},
    {0xE8, {0x00,0x0E}, 2},
    {0xFF, {0x77,0x01,0x00,0x00,0x00}, 5},




    {0x11, {0X00}, 1},


    {0xFF, {0x77,0x01,0x00,0x00,0x13}, 5},
    {0xE8, {0x00,0x0C}, 2},
    {0xE8, {0x00,0x00}, 2},
    {0xFF, {0x77,0x01,0x00,0x00,0x00}, 5},

    {0x36, {0x00}, 1},
    {0x3A, {0x66}, 1},//0x77//改颜色的  0x66  两个都能正常显示了 2.1  2.76共用一套
  //  {0x29, {0x00}, 1},
    {0x00, {0x00}, 0xff},

    #else

      //2.1不带触摸
    {0xF0, {0x55, 0xAA, 0x52, 0x08, 0x00}, 5},
    {0xF6, {0x5A, 0x87}, 2},
    {0xC1, {0x3F}, 1},
    {0xCD, {0x25}, 1},
    {0xC9, {0x10}, 1},
    {0xF8, {0x8A}, 1},
    {0xAC, {0x45}, 1},
    {0xA7, {0x47}, 1},
    {0xA0, {0xDD}, 1},//0xFF 20200413 James
    {0x87, {0x04, 0x03, 0x66}, 3},
    {0x86, {0x99, 0xa3, 0xa3, 0x51}, 4},//0X71 20200413 James
    {0xFA, {0x08, 0x08, 0x08, 0x04}, 4},
    //-----for vcom-----//
    {0x9A, {0x8a}, 1},  //4A
    {0x9B, {0x62}, 1},   //22
    {0x82, {0x48, 0x48}, 2},  //08-08
    //-----for vcom-----//
    {0xB1, {0x10}, 1},
    {0x7A, {0x13, 0x1A}, 2},
    {0x7B, {0x13, 0x1A}, 2},
    {0x6D, {0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x02, 0x0b, 0x01, 0x00, 0x1f, 0x1e, 0x09, 0x0f, 0x1e, 0x1e, 0x1e, 0x1e,
            0x10, 0x0a, 0x1e, 0x1f, 0x00, 0x08, 0x0b, 0x02, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e}, 32},
    {0x64, {0x18, 0x07, 0x01, 0xE7, 0x03, 0x03, 0x18, 0x06, 0x01, 0xE6, 0x03, 0x03, 0x7a, 0x7a, 0x7a, 0x7a}, 16},
    {0x65, {0x58, 0x26, 0x18, 0x2c, 0x03, 0x03, 0x58, 0x26, 0x18, 0x2c, 0x03, 0x03, 0x7a, 0x7a, 0x7a, 0x7a}, 16},
    {0x66, {0x58, 0x26, 0x18, 0x2c, 0x03, 0x03, 0x58, 0x26, 0x18, 0x2c, 0x03, 0x03, 0x7a, 0x7a, 0x7a, 0x7a}, 16},
    {0x67, {0x18, 0x05, 0x01, 0xE5, 0x03, 0x03, 0x18, 0x04, 0x01, 0xE4, 0x03, 0x03, 0x7a, 0x7a, 0x7a, 0x7a}, 16},
    {0x60, {0x18, 0x09, 0x7A, 0x7A, 0x51, 0xF1, 0x7A, 0x7A}, 8},
    {0x63, {0x51, 0xF1, 0x7A, 0x7A, 0x18, 0x08, 0x7A, 0x7A}, 8},
    {0xD1, {0x00, 0x00, 0x00, 0x0E, 0x00, 0x31, 0x00, 0x4E, 0x00, 0x67, 0x00, 0x92, 0x00, 0xB5, 0x00, 0xED, 0x01, 0x1C,
            0x01, 0x66, 0x01, 0xA4, 0x02, 0x04, 0x02, 0x53, 0x02, 0x56, 0x02, 0x9F, 0x02, 0xF3, 0x03, 0x29, 0x03, 0x73,
            0x03, 0xA1, 0x03, 0xB9, 0x03, 0xC8, 0x03, 0xDB, 0x03, 0xE7, 0x03, 0xF4, 0x03, 0xFB, 0x03, 0XFF}, 52},
    {0xD2, {0x00, 0x00, 0x00, 0x0E, 0x00, 0x31, 0x00, 0x4E, 0x00, 0x67, 0x00, 0x92, 0x00, 0xB5, 0x00, 0xED, 0x01, 0x1C,
            0x01, 0x66, 0x01, 0xA4, 0x02, 0x04, 0x02, 0x53, 0x02, 0x56, 0x02, 0x9F, 0x02, 0xF3, 0x03, 0x29, 0x03, 0x73,
            0x03, 0xA1, 0x03, 0xB9, 0x03, 0xC8, 0x03, 0xDB, 0x03, 0xE7, 0x03, 0xF4, 0x03, 0xFB, 0x03, 0XFF}, 52},
    {0xD3, {0x00, 0x00, 0x00, 0x0E, 0x00, 0x31, 0x00, 0x4E, 0x00, 0x67, 0x00, 0x92, 0x00, 0xB5, 0x00, 0xED, 0x01, 0x1C,
            0x01, 0x66, 0x01, 0xA4, 0x02, 0x04, 0x02, 0x53, 0x02, 0x56, 0x02, 0x9F, 0x02, 0xF3, 0x03, 0x29, 0x03, 0x73,
            0x03, 0xA1, 0x03, 0xB9, 0x03, 0xC8, 0x03, 0xDB, 0x03, 0xE7, 0x03, 0xF4, 0x03, 0xFB, 0x03, 0XFF}, 52},
    {0xD4, {0x00, 0x00, 0x00, 0x0E, 0x00, 0x31, 0x00, 0x4E, 0x00, 0x67, 0x00, 0x92, 0x00, 0xB5, 0x00, 0xED, 0x01, 0x1C,
            0x01, 0x66, 0x01, 0xA4, 0x02, 0x04, 0x02, 0x53, 0x02, 0x56, 0x02, 0x9F, 0x02, 0xF3, 0x03, 0x29, 0x03, 0x73,
            0x03, 0xA1, 0x03, 0xB9, 0x03, 0xC8, 0x03, 0xDB, 0x03, 0xE7, 0x03, 0xF4, 0x03, 0xFB, 0x03, 0XFF}, 52},
    {0xD5, {0x00, 0x00, 0x00, 0x0E, 0x00, 0x31, 0x00, 0x4E, 0x00, 0x67, 0x00, 0x92, 0x00, 0xB5, 0x00, 0xED, 0x01, 0x1C,
            0x01, 0x66, 0x01, 0xA4, 0x02, 0x04, 0x02, 0x53, 0x02, 0x56, 0x02, 0x9F, 0x02, 0xF3, 0x03, 0x29, 0x03, 0x73,
            0x03, 0xA1, 0x03, 0xB9, 0x03, 0xC8, 0x03, 0xDB, 0x03, 0xE7, 0x03, 0xF4, 0x03, 0xFB, 0x03, 0XFF}, 52},
    {0xD6, {0x00, 0x00, 0x00, 0x0E, 0x00, 0x31, 0x00, 0x4E, 0x00, 0x67, 0x00, 0x92, 0x00, 0xB5, 0x00, 0xED, 0x01, 0x1C,
            0x01, 0x66, 0x01, 0xA4, 0x02, 0x04, 0x02, 0x53, 0x02, 0x56, 0x02, 0x9F, 0x02, 0xF3, 0x03, 0x29, 0x03, 0x73,
            0x03, 0xA1, 0x03, 0xB9, 0x03, 0xC8, 0x03, 0xDB, 0x03, 0xE7, 0x03, 0xF4, 0x03, 0xFB, 0x03, 0XFF}, 52},
    {0x11, {0x00}, 0},
    {0x00, {0x00}, 0xff},

    #endif







};

/**
 * @brief Configure LCD with specific commands and data
 *
 * @return
 *      - ESP_OK: Success, otherwise returns ESP_ERR_xxx
 *
 */
static esp_err_t lcd_config(void)
{
    gpio_config_t config = {
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pin_bit_mask = BIT64(BSP_LCD_SPI_CS) | BIT64(BSP_LCD_SPI_SCK) | BIT64(BSP_LCD_SPI_SDO) | BIT64(BSP_LCD_RST) | BIT64(BSP_LCD_BL),
    };
    ESP_RETURN_ON_ERROR(gpio_config(&config), TAG, "SPI GPIO config failed");
    gpio_set_level(BSP_LCD_RST, 1);
    CS(1);
    SCK(1);
    SDO(1);

    uint8_t i = 0;
    while (LCD_CONFIG_CMD[i].data_bytes != 0xff) {
        BSP_ERROR_CHECK_RETURN_ERR(spi_write_cmd(LCD_CONFIG_CMD[i].cmd));
        for (uint8_t j = 0; j < LCD_CONFIG_CMD[i].data_bytes; j++) {
            BSP_ERROR_CHECK_RETURN_ERR(spi_write_data(LCD_CONFIG_CMD[i].data[j]));
        }
        i++;
    }
   
    vTaskDelay(pdMS_TO_TICKS(120));
    BSP_ERROR_CHECK_RETURN_ERR(spi_write_cmd(0x29));
    vTaskDelay(pdMS_TO_TICKS(20));

    // gpio_reset_pin(BSP_LCD_SPI_CS);
    gpio_reset_pin(BSP_LCD_SPI_SCK);
    gpio_reset_pin(BSP_LCD_SPI_SDO);

    return ESP_OK;
}

static const button_config_t bsp_encoder_btn_config = {
    .type = BUTTON_TYPE_GPIO,
    .gpio_button_config.active_level = false,
    .gpio_button_config.gpio_num = BSP_BTN_PRESS,
};

static const knob_config_t bsp_encoder_a_b_config = {
    .default_direction = 0,
    .gpio_encoder_a = BSP_ENCODER_A,
    .gpio_encoder_b = BSP_ENCODER_B,
};

esp_err_t app_lcd_init(void)
{
    esp_err_t ret = ESP_OK;

    BSP_ERROR_CHECK_RETURN_ERR(lcd_config());
    /* LCD initialization */
    ESP_LOGI(TAG, "Initialize RGB panel");
    esp_lcd_rgb_panel_config_t panel_conf = {
        .clk_src = LCD_CLK_SRC_PLL160M,
        .psram_trans_align = 64,
        .data_width = 16,
        .bits_per_pixel = 16,
        .de_gpio_num = BSP_LCD_DE,
        .pclk_gpio_num = BSP_LCD_PCLK,
        .vsync_gpio_num = BSP_LCD_VSYNC,
        .hsync_gpio_num = BSP_LCD_HSYNC,
        .disp_gpio_num = BSP_LCD_DISP,
        .data_gpio_nums = {
            BSP_LCD_DATA0,
            BSP_LCD_DATA1,
            BSP_LCD_DATA2,
            BSP_LCD_DATA3,
            BSP_LCD_DATA4,
            BSP_LCD_DATA5,
            BSP_LCD_DATA6,
            BSP_LCD_DATA7,
            BSP_LCD_DATA8,
            BSP_LCD_DATA9,
            BSP_LCD_DATA10,
            BSP_LCD_DATA11,
            BSP_LCD_DATA12,
            BSP_LCD_DATA13,
            BSP_LCD_DATA14,
            BSP_LCD_DATA15,
        },
        .timings = EXAMPLE_LCD_PANEL_35HZ_RGB_TIMING(),
        .flags.fb_in_psram = 1,
        .num_fbs = EXAMPLE_LCD_RGB_BUFFER_NUMS,
#if EXAMPLE_LCD_RGB_BOUNCE_BUFFER_MODE
        .bounce_buffer_size_px = BSP_LCD_H_RES * EXAMPLE_LCD_RGB_BOUNCE_BUFFER_HEIGHT,
#endif
    };
    ESP_GOTO_ON_ERROR(esp_lcd_new_rgb_panel(&panel_conf, &lcd_panel), err, TAG, "RGB init failed");
    ESP_GOTO_ON_ERROR(esp_lcd_panel_init(lcd_panel), err, TAG, "LCD init failed");

    return ret;

err:
    if (lcd_panel) {
        esp_lcd_panel_del(lcd_panel);
    }
    return ret;
}
static esp_lcd_touch_handle_t touch_handle = NULL;
/* LVGL display and touch */
static lv_display_t *lvgl_disp2 = NULL;
static lv_indev_t *lvgl_touch_indev = NULL;
 esp_err_t app_touch_init(void)
{
    /* Initilize I2C */
    const i2c_config_t i2c_conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = EXAMPLE_PIN_NUM_TOUCH_SDA,
        .sda_pullup_en = GPIO_PULLUP_DISABLE,
        .scl_io_num = EXAMPLE_PIN_NUM_TOUCH_SCL,
        .scl_pullup_en = GPIO_PULLUP_DISABLE,
        .master.clk_speed = 400 * 1000
    };
    i2c_param_config((esp_lcd_i2c_bus_handle_t)TOUCH_HOST, &i2c_conf);
    i2c_driver_install((esp_lcd_i2c_bus_handle_t)TOUCH_HOST, i2c_conf.mode, 0, 0, 0);

    /* Initialize touch HW */
    const esp_lcd_touch_config_t tp_cfg = {
        .x_max = BSP_LCD_H_RES,
        .y_max = BSP_LCD_V_RES,
        .rst_gpio_num = GPIO_NUM_NC, // Shared with LCD reset
        .int_gpio_num = EXAMPLE_PIN_NUM_TOUCH_INT,
        .levels = {
            .reset = 0,
            .interrupt = 0,
        },
        .flags = {
            .swap_xy = 0,
            .mirror_x = 0,
            .mirror_y = 0,
        },
    };
    esp_lcd_panel_io_handle_t tp_io_handle = NULL;
    const esp_lcd_panel_io_i2c_config_t tp_io_config = ESP_LCD_TOUCH_IO_I2C_CST816S_CONFIG();
    esp_lcd_new_panel_io_i2c((esp_lcd_i2c_bus_handle_t)TOUCH_HOST, &tp_io_config, &tp_io_handle);
    return esp_lcd_touch_new_i2c_cst816s(tp_io_handle, &tp_cfg, &touch_handle);

    

}

esp_err_t lvgl_port_display_init()
{
     /* Initialize LVGL */
    const lvgl_port_cfg_t lvgl_cfg = {
        .task_priority = 4,         /* LVGL task priority */
        .task_stack = 6144*2,         /* LVGL task stack size */
        .task_affinity = -1,        /* LVGL task pinned to core (-1 is no affinity) */
        .task_max_sleep_ms = 500,   /* Maximum sleep in LVGL task */
        .timer_period_ms = 5        /* LVGL timer tick period in ms */
    };
    esp_err_t ret=lvgl_port_init(&lvgl_cfg);
    ESP_RETURN_ON_ERROR(ret, TAG, "LVGL port initialization failed");

    uint32_t buff_size = BSP_LCD_H_RES * EXAMPLE_LCD_DRAW_BUFF_HEIGHT;
#if EXAMPLE_LCD_LVGL_FULL_REFRESH || EXAMPLE_LCD_LVGL_DIRECT_MODE
    buff_size = BSP_LCD_H_RES * BSP_LCD_V_RES;
#endif

    /* Add LCD screen */
    ESP_LOGD(TAG, "Add LCD screen");
    const lvgl_port_display_cfg_t disp_cfg = {
        .panel_handle = lcd_panel,
        .buffer_size = buff_size,
        .double_buffer = EXAMPLE_LCD_DRAW_BUFF_DOUBLE,
        .hres = BSP_LCD_H_RES,
        .vres = BSP_LCD_V_RES,
        .monochrome = false,
#if LVGL_VERSION_MAJOR >= 9
        .color_format = LV_COLOR_FORMAT_RGB565,
#endif
        .rotation = {
            .swap_xy = false,//false
            .mirror_x = false,
            .mirror_y = false,
        },
        .flags = {
            .buff_dma = false,//false
            .buff_spiram = false,
#if EXAMPLE_LCD_LVGL_FULL_REFRESH
            .full_refresh = true,
#elif EXAMPLE_LCD_LVGL_DIRECT_MODE
            .direct_mode = true,
#endif
#if LVGL_VERSION_MAJOR >= 9
            .swap_bytes = false,
#endif
        }
    };
    const lvgl_port_display_rgb_cfg_t rgb_cfg = {
        .flags = {
#if EXAMPLE_LCD_RGB_BOUNCE_BUFFER_MODE
            .bb_mode = true,
#else
            .bb_mode = false,
#endif
#if EXAMPLE_LCD_LVGL_AVOID_TEAR
            .avoid_tearing = true,
#else
            .avoid_tearing = false,
#endif
        }
    };
    lvgl_disp = lvgl_port_add_disp_rgb(&disp_cfg, &rgb_cfg);

    /* Add touch input (for selected screen) */  // 按钮
    // const lvgl_port_encoder_cfg_t encoder_cfg = {
    //     .disp = lvgl_disp,
    //     .encoder_a_b= &bsp_encoder_a_b_config,
    //     .encoder_enter = &bsp_encoder_btn_config
    // };
    // lvgl_encoder_indev = lvgl_port_add_encoder(&encoder_cfg);


  
    /* Add touch input (for selected screen) */   //触摸
    const lvgl_port_touch_cfg_t touch_cfg = {
        .disp = lvgl_disp,
        .handle = touch_handle,
    };
    lvgl_touch_indev = lvgl_port_add_touch(&touch_cfg);

    return ESP_OK;
}

//***************The following code is for standardizing the event triggering of rotary encoders and buttons.**************** */
//**************旋钮********* */
extern void   LVGL_knob_event(void *event);
extern void   LVGL_button_event(void *event);
static knob_handle_t knob = NULL;

const char *knob_event_table[] = {
    "KNOB_LEFT",
    "KNOB_RIGHT",
    "KNOB_H_LIM",
    "KNOB_L_LIM",
    "KNOB_ZERO",
};

static void knob_event_cb(void *arg, void *data)
{
    //ESP_LOGI(TAG, "knob event %s, %d", knob_event_table[(knob_event_t)data], iot_knob_get_count_value(knob));
    LVGL_knob_event(data);
   
}

void knob_init(uint32_t encoder_a, uint32_t encoder_b)
{
    knob_config_t cfg = {
        .default_direction = 0,
        .gpio_encoder_a = encoder_a,
        .gpio_encoder_b = encoder_b,
#if CONFIG_PM_ENABLE
        .enable_power_save = true,
#endif
    };

    knob = iot_knob_create(&cfg);
    assert(knob);
    esp_err_t err = iot_knob_register_cb(knob, KNOB_LEFT, knob_event_cb, (void *)KNOB_LEFT);
    err |= iot_knob_register_cb(knob, KNOB_RIGHT, knob_event_cb, (void *)KNOB_RIGHT);
    err |= iot_knob_register_cb(knob, KNOB_H_LIM, knob_event_cb, (void *)KNOB_H_LIM);
    err |= iot_knob_register_cb(knob, KNOB_L_LIM, knob_event_cb, (void *)KNOB_L_LIM);
    err |= iot_knob_register_cb(knob, KNOB_ZERO, knob_event_cb, (void *)KNOB_ZERO);
    ESP_ERROR_CHECK(err);
}

//******************** */

//*********按键**** */
#if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32C2 || CONFIG_IDF_TARGET_ESP32H2 || CONFIG_IDF_TARGET_ESP32C6
#define BOOT_BUTTON_NUM         9
#else
#define BOOT_BUTTON_NUM         0
#endif
#define BUTTON_ACTIVE_LEVEL     0
const char *button_event_table[] = {
    "BUTTON_PRESS_DOWN",
    "BUTTON_PRESS_UP",
    "BUTTON_PRESS_REPEAT",
    "BUTTON_PRESS_REPEAT_DONE",
    "BUTTON_SINGLE_CLICK",
    "BUTTON_DOUBLE_CLICK",
    "BUTTON_MULTIPLE_CLICK",
    "BUTTON_LONG_PRESS_START",
    "BUTTON_LONG_PRESS_HOLD",
    "BUTTON_LONG_PRESS_UP",
    "BUTTON_PRESS_END",
};

static void button_event_cb(void *arg, void *data)
{
    ESP_LOGI(TAG, "Button event %s", button_event_table[(button_event_t)data]);
    LVGL_button_event(data);
  
  
}
void button_init(uint32_t button_num)
{
    button_config_t btn_cfg = {
        .type = BUTTON_TYPE_GPIO,
        .gpio_button_config = {
            .gpio_num = button_num,
            .active_level = BUTTON_ACTIVE_LEVEL,
#if CONFIG_GPIO_BUTTON_SUPPORT_POWER_SAVE
            .enable_power_save = true,
#endif
        },
    };
    button_handle_t btn = iot_button_create(&btn_cfg);
    assert(btn);
    esp_err_t err = iot_button_register_cb(btn, BUTTON_PRESS_DOWN, button_event_cb, (void *)BUTTON_PRESS_DOWN);
    err |= iot_button_register_cb(btn, BUTTON_PRESS_UP, button_event_cb, (void *)BUTTON_PRESS_UP);
    err |= iot_button_register_cb(btn, BUTTON_PRESS_REPEAT, button_event_cb, (void *)BUTTON_PRESS_REPEAT);
    err |= iot_button_register_cb(btn, BUTTON_PRESS_REPEAT_DONE, button_event_cb, (void *)BUTTON_PRESS_REPEAT_DONE);
    err |= iot_button_register_cb(btn, BUTTON_SINGLE_CLICK, button_event_cb, (void *)BUTTON_SINGLE_CLICK);
    err |= iot_button_register_cb(btn, BUTTON_DOUBLE_CLICK, button_event_cb, (void *)BUTTON_DOUBLE_CLICK);
    err |= iot_button_register_cb(btn, BUTTON_LONG_PRESS_START, button_event_cb, (void *)BUTTON_LONG_PRESS_START);
    err |= iot_button_register_cb(btn, BUTTON_LONG_PRESS_HOLD, button_event_cb, (void *)BUTTON_LONG_PRESS_HOLD);
    err |= iot_button_register_cb(btn, BUTTON_LONG_PRESS_UP, button_event_cb, (void *)BUTTON_LONG_PRESS_UP);
    err |= iot_button_register_cb(btn, BUTTON_PRESS_END, button_event_cb, (void *)BUTTON_PRESS_END);

#if CONFIG_ENTER_LIGHT_SLEEP_MODE_MANUALLY
    /*!< For enter Power Save */
    button_power_save_config_t config = {
        .enter_power_save_cb = button_enter_power_save,
    };
    err |= iot_button_register_power_save_cb(&config);
#endif

    ESP_ERROR_CHECK(err);
}



//***************** */



