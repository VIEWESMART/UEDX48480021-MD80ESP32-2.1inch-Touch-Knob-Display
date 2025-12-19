/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#pragma once

#include "driver/spi_common.h"
#include "driver/gpio.h"
#include "lvgl.h"

#include "sdkconfig.h"

/**************************************************************************************************
 *  BSP Pinout
 **************************************************************************************************/
/* Display */
#define BSP_LCD_VSYNC           (GPIO_NUM_3)
#define BSP_LCD_HSYNC           (GPIO_NUM_46)
#define BSP_LCD_DE              (GPIO_NUM_17)
#define BSP_LCD_RST             (GPIO_NUM_8)
#define BSP_LCD_PCLK            (GPIO_NUM_9)
#define BSP_LCD_DISP            (GPIO_NUM_NC)
#define BSP_LCD_DATA0           (GPIO_NUM_10)   // B3
#define BSP_LCD_DATA1           (GPIO_NUM_11)   // B4
#define BSP_LCD_DATA2           (GPIO_NUM_12)   // B5
#define BSP_LCD_DATA3           (GPIO_NUM_13)   // B6
#define BSP_LCD_DATA4           (GPIO_NUM_14)   // B7
#define BSP_LCD_DATA5           (GPIO_NUM_21)   // G2
#define BSP_LCD_DATA6           (GPIO_NUM_47)   // G3
#define BSP_LCD_DATA7           (GPIO_NUM_48)   // G4
#define BSP_LCD_DATA8           (GPIO_NUM_45)   // G5
#define BSP_LCD_DATA9           (GPIO_NUM_38)   // G6
#define BSP_LCD_DATA10          (GPIO_NUM_39)   // G7
#define BSP_LCD_DATA11          (GPIO_NUM_40)   // R3
#define BSP_LCD_DATA12          (GPIO_NUM_41)   // R4
#define BSP_LCD_DATA13          (GPIO_NUM_42)   // R5
#define BSP_LCD_DATA14          (GPIO_NUM_2)    // R6
#define BSP_LCD_DATA15          (GPIO_NUM_1)    // R7
#define BSP_LCD_SPI_CS          (GPIO_NUM_18)
#define BSP_LCD_SPI_SCK         (GPIO_NUM_13)
#define BSP_LCD_SPI_SDO         (GPIO_NUM_12)




#define BSP_LCD_BL              (GPIO_NUM_7)


#define  VIEWE_DX48480021E_WB_A_with_touch 1

/* Buttons */
typedef enum {
    BSP_BTN_PRESS = GPIO_NUM_0,
} bsp_button_t;

#define BSP_ENCODER_A         (GPIO_NUM_6)
#define BSP_ENCODER_B         (GPIO_NUM_5)

#ifdef __cplusplus
extern "C" {
#endif


#define TOUCH_HOST                      (I2C_NUM_0)
#define EXAMPLE_PIN_NUM_TOUCH_SCL         (GPIO_NUM_15)
#define EXAMPLE_PIN_NUM_TOUCH_SDA         (GPIO_NUM_16)
#define EXAMPLE_PIN_NUM_TOUCH_RST         (GPIO_NUM_NC)
#define EXAMPLE_PIN_NUM_TOUCH_INT         (GPIO_NUM_NC)

/********************************************************************************************************************************
 *
 * Display Interface
 *
 * LVGL is used as graphics library. LVGL is NOT thread safe, therefore the user must take LVGL mutex
 * by calling `bsp_display_lock()` before calling any LVGL API (lv_...) and then give the mutex with
 * `bsp_display_unlock()`.
 *
 *******************************************************************************************************************************/
/* LCD related parameters */
#define BSP_LCD_H_RES                   (480)
#define BSP_LCD_V_RES                   (480)
#define BSP_LCD_PIXEL_CLOCK_HZ          (26 * 1000 * 1000)
#define BSP_LCD_HSYNC_BACK_PORCH        (20)
#define BSP_LCD_HSYNC_FRONT_PORCH       (40)
#define BSP_LCD_HSYNC_PULSE_WIDTH       (8)
#define BSP_LCD_VSYNC_BACK_PORCH        (20)
#define BSP_LCD_VSYNC_FRONT_PORCH       (50)
#define BSP_LCD_VSYNC_PULSE_WIDTH       (8)
#define BSP_LCD_PCLK_ACTIVE_NEG         (false)

#if VIEWE_DX48480021E_WB_A_with_touch
//有触摸的PLCK
#define EXAMPLE_LCD_PANEL_35HZ_RGB_TIMING()  \
    {                                               \
        .pclk_hz = 18 * 1000 * 1000,                \
        .h_res = BSP_LCD_H_RES,                     \
        .v_res = BSP_LCD_V_RES,                     \
        .hsync_pulse_width = 8,                     \
        .hsync_back_porch = 20,                     \
        .hsync_front_porch = 40,                    \
        .vsync_pulse_width = 8,                     \
        .vsync_back_porch = 20,                     \
        .vsync_front_porch = 50,                    \
        .flags.pclk_active_neg = false,             \
    }

#else
//无触摸的PLCK
#define EXAMPLE_LCD_PANEL_35HZ_RGB_TIMING()  \
    {                                               \
        .pclk_hz = 20 * 1000 * 1000,                \
        .h_res = BSP_LCD_H_RES,                     \
        .v_res = BSP_LCD_V_RES,                     \
        .hsync_pulse_width = 8,                     \
        .hsync_back_porch = 20,                     \
        .hsync_front_porch = 40,                    \
        .vsync_pulse_width = 8,                     \
        .vsync_back_porch = 20,                     \
        .vsync_front_porch = 50,                    \
        .flags.pclk_active_neg = false,             \
    }
#endif
/* LCD settings */
#define EXAMPLE_LCD_LVGL_FULL_REFRESH           (1)
#define EXAMPLE_LCD_LVGL_DIRECT_MODE            (0)
#define EXAMPLE_LCD_LVGL_AVOID_TEAR             (1)
#define EXAMPLE_LCD_RGB_BOUNCE_BUFFER_MODE      (1)
#define EXAMPLE_LCD_DRAW_BUFF_DOUBLE            (1)
#define EXAMPLE_LCD_DRAW_BUFF_HEIGHT            (100)
#define EXAMPLE_LCD_RGB_BUFFER_NUMS             (2)
#define EXAMPLE_LCD_RGB_BOUNCE_BUFFER_HEIGHT    (30)

/* LVGL related parameters */
#define LVGL_TICK_PERIOD_MS         (CONFIG_BSP_DISPLAY_LVGL_TICK)
#define LVGL_BUFFER_HEIGHT          (CONFIG_BSP_DISPLAY_LVGL_BUF_HEIGHT)
#if CONFIG_BSP_DISPLAY_LVGL_PSRAM
#define LVGL_BUFFER_MALLOC          (MALLOC_CAP_SPIRAM)
#else
#define LVGL_BUFFER_MALLOC          (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#endif

/**
 * @brief Initialize display
 *
 * @note This function initializes display controller and starts LVGL handling task.
 * @note Users can get LCD panel handle from `user_data` in returned display.
 *
 * @return Pointer to LVGL display or NULL when error occured
 */

esp_err_t app_lcd_init(void);
esp_err_t lvgl_port_display_init();
esp_err_t app_touch_init(void);


void knob_init(uint32_t encoder_a, uint32_t encoder_b);
void button_init(uint32_t button_num);




#ifdef __cplusplus
}
#endif
