#include <Arduino.h>
#include <ESP_Panel_Library.h>
#include <lvgl.h>
#include "lvgl_port_v8.h"
#include <ESP_Knob.h>
#include <Button.h>
#include <ui.h>
/**
/* To use the built-in examples and demos of LVGL uncomment the includes below respectively.
 * You also need to copy `lvgl/examples` to `lvgl/src/examples`. Similarly for the demos `lvgl/demos` to `lvgl/src/demos`.
 */
// #include <demos/lv_demos.h>
// #include <examples/lv_examples.h>
#ifdef KNOB21
#define GPIO_NUM_KNOB_PIN_A     6
#define GPIO_NUM_KNOB_PIN_B     5
#define GPIO_BUTTON_PIN         GPIO_NUM_0
#endif
#ifdef KNOB13
#define GPIO_NUM_KNOB_PIN_A     7
#define GPIO_NUM_KNOB_PIN_B     6
#define GPIO_BUTTON_PIN         GPIO_NUM_9
#endif
ESP_Knob *knob;


/*Encoder and button event handling*/
void onKnobLeftEventCallback(int count, void *usr_data)
{
    Serial.printf("Detect left event, count is %d\n", count);
    lvgl_port_lock(-1);
    LVGL_knob_event((void*)KNOB_LEFT);
    lvgl_port_unlock();
}

void onKnobRightEventCallback(int count, void *usr_data)
{
    Serial.printf("Detect right event, count is %d\n", count);
    lvgl_port_lock(-1);
    LVGL_knob_event((void*)KNOB_RIGHT);
    lvgl_port_unlock();
}

static void SingleClickCb(void *button_handle, void *usr_data) {
    Serial.println("Button Single Click");
    lvgl_port_lock(-1);
    LVGL_button_event((void*)BUTTON_SINGLE_CLICK);
    lvgl_port_unlock();
}

static void DoubleClickCb(void *button_handle, void *usr_data)
{
    Serial.println("Button Double Click");
}

static void LongPressStartCb(void *button_handle, void *usr_data) {
    Serial.println("Button Long Press Start");
    lvgl_port_lock(-1);
    LVGL_button_event((void*)BUTTON_LONG_PRESS_START);
    lvgl_port_unlock();
}

/*device initialization*/
void setup()
{
    String title = "LVGL porting example";
#ifdef IM
    pinMode(IM1, OUTPUT);
    digitalWrite(IM1, HIGH);
  #ifdef BOARD_VIEWE_ESP_S3_Touch_LCD_35_V2
    pinMode(IM0, OUTPUT);
    digitalWrite(IM0, HIGH);
  #endif
  #ifndef BOARD_VIEWE_ESP_S3_Touch_LCD_35_V2
    pinMode(IM0, OUTPUT);
    digitalWrite(IM0, LOW);
  #endif
#endif

    Serial.begin(115200);
    Serial.println(title + " start");

    Serial.println("Initialize panel device");
    ESP_Panel *panel = new ESP_Panel();
    panel->init();
#if LVGL_PORT_AVOID_TEAR
    // When avoid tearing function is enabled, configure the bus according to the LVGL configuration
    ESP_PanelBus *lcd_bus = panel->getLcd()->getBus();
#if ESP_PANEL_LCD_BUS_TYPE == ESP_PANEL_BUS_TYPE_RGB
    static_cast<ESP_PanelBus_RGB *>(lcd_bus)->configRgbBounceBufferSize(LVGL_PORT_RGB_BOUNCE_BUFFER_SIZE);
    static_cast<ESP_PanelBus_RGB *>(lcd_bus)->configRgbFrameBufferNumber(LVGL_PORT_DISP_BUFFER_NUM);
#elif ESP_PANEL_LCD_BUS_TYPE == ESP_PANEL_BUS_TYPE_MIPI_DSI
    static_cast<ESP_PanelBus_DSI *>(lcd_bus)->configDpiFrameBufferNumber(LVGL_PORT_DISP_BUFFER_NUM);
#endif
#endif
    panel->begin();

    Serial.println("Initialize Knob device");
    knob = new ESP_Knob(GPIO_NUM_KNOB_PIN_A, GPIO_NUM_KNOB_PIN_B);
    knob->begin();
    knob->attachLeftEventCallback(onKnobLeftEventCallback);
    knob->attachRightEventCallback(onKnobRightEventCallback);

    Serial.println("Initialize Button device");
    Button *btn = new Button(GPIO_BUTTON_PIN, false);

    btn->attachSingleClickEventCb(&SingleClickCb, NULL);
    btn->attachDoubleClickEventCb(&DoubleClickCb, NULL);
    btn->attachLongPressStartEventCb(&LongPressStartCb, NULL);

    Serial.println("Initialize LVGL");
    lvgl_port_init(panel->getLcd(), panel->getTouch());

    Serial.println("Create UI");
    /* Lock the mutex due to the LVGL APIs are not thread-safe */
    lvgl_port_lock(-1);

    /**
     * Create a simple label
     *
     */
    // lv_obj_t *label = lv_label_create(lv_scr_act());
    // lv_label_set_text(label, title.c_str());
    // lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
  
    /**
     * Try an example. Don't forget to uncomment header.
     * See all the examples online: https://docs.lvgl.io/master/examples.html
     * source codes: https://github.com/lvgl/lvgl/tree/e7f88efa5853128bf871dde335c0ca8da9eb7731/examples
     */
    //  lv_example_btn_1();

    /**
     * Or try out a demo.
     * Don't forget to uncomment header and enable the demos in `lv_conf.h`. E.g. `LV_USE_DEMO_WIDGETS`
     */
    // lv_demo_widgets();
    // lv_demo_benchmark();
    // lv_demo_music();
    // lv_demo_stress();
    ui_init();

    /* Release the mutex */
    lvgl_port_unlock();

    Serial.println(title + " end");
}

void loop()
{
    Serial.println("IDLE loop");
    delay(50);
}
