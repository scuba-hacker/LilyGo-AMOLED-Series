/**
 * @file      LVGL_Rotation.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2023  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2023-07-14
 * @note      Screen rotation only supports 1.91-inch touch and non-touch versions
 * @note      Screen rotation only supports 1.91-inch touch and non-touch versions
 */
#ifdef LILYGO_TWRITSTBAND_S3
#error  "Current example does not apply to T-Wristband"
#endif
#include <LilyGo_AMOLED.h>
#include <LV_Helper.h>
#include <AceButton.h>

using namespace ace_button;

LilyGo_Class amoled;
lv_obj_t *label1;
lv_obj_t *label2;
uint8_t btnPin = 0;
AceButton button(btnPin);
uint8_t rotation = 1;

const char *formart_string = "#0000ff X:%d#\n #ff00ff Y:%d#\n #f00f00 Rotation:%d#\n #00ff00 Size:%s# ";

void setRotation()
{
    amoled.setRotation(rotation++);
    rotation %= 4;
    lv_disp_drv_t *drv = lv_disp_get_default()->driver;
    drv->hor_res = amoled.width();
    drv->ver_res = amoled.height();
    lv_disp_drv_update(lv_disp_get_default(), drv);
}

void handleEvent(AceButton * /* button */, uint8_t eventType,
                 uint8_t /* buttonState */)
{
    switch (eventType) {
    case AceButton::kEventPressed:
        setRotation();
        break;
    default: break;
    }
}


static void event_cb(lv_event_t *e)
{
    setRotation();
}



void setup(void)
{
    Serial.begin(115200);

    bool rslt = false;

    // 1.47 inches does not support rotation, this sketch cannot be used
    //rslt = amoled.beginAMOLED_147();

    // Begin LilyGo  1.91 Inch AMOLED board class
    //rslt =  amoled.beginAMOLED_191();

    // 2.41 inches has not yet been adapted.
    //rslt =  amoled.beginAMOLED_241();

    // Automatically determine the access device
    rslt = amoled.begin();

    if (!rslt) {
        while (1) {
            Serial.println("The board model cannot be detected, please raise the Core Debug Level to an error");
            delay(1000);
        }
    }

    // Register lvgl helper
    beginLvglHelper(amoled);


    label1 = lv_label_create(lv_scr_act());
    lv_label_set_long_mode(label1, LV_LABEL_LONG_WRAP);     /*Break the long lines*/
    lv_label_set_recolor(label1, true);                      /*Enable re-coloring by commands in the text*/
    lv_label_set_text_fmt(label1, formart_string, 0, 0, amoled.getRotation(), amoled.getName());
    lv_obj_set_width(label1, 150);  /*Set smaller width to make the lines wrap*/
    lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label1, LV_ALIGN_CENTER, 0, -40);


    label2 = lv_label_create(lv_scr_act());
    lv_label_set_long_mode(label2, LV_LABEL_LONG_SCROLL_CIRCULAR); /*Circular scroll*/
    lv_obj_set_width(label2, 150);
    lv_label_set_text(label2, "Press the yellow touch button on the screen to trigger");
    lv_obj_align(label2, LV_ALIGN_CENTER, 0, 40);

    // Only 1.91 Inch AMOLED board support
    amoled.setHomeButtonCallback([](void *ptr) {
        Serial.println("Home key pressed!");
        static uint32_t checkMs = 0;
        if (millis() > checkMs) {
            lv_label_set_text(label2, "Home Pressed");
        }
        checkMs = millis() + 200;
        lv_timer_create([](lv_timer_t *t) {
            lv_label_set_text(label2, "Press the yellow touch button on the screen to trigger");
            lv_timer_del(t);
        }, 2000, NULL);
    }, NULL);

    lv_obj_t *btn = lv_btn_create(lv_scr_act());
    lv_obj_t *label_str = lv_label_create(btn);
    lv_label_set_text(label_str, "setRotation");
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 80);
    lv_obj_add_event_cb(btn, event_cb, LV_EVENT_CLICKED, NULL);

    //Initial BOOT button, used as setting direction trigger
    pinMode(btnPin, INPUT_PULLUP);
    button.setEventHandler(handleEvent);
}

void loop()
{
    static int16_t x, y;
    uint8_t rotation = amoled.getRotation();
    bool touched = amoled.getPoint(&x, &y);
    if ( touched ) {
        lv_label_set_text_fmt(label1, formart_string, x, y, amoled.getRotation(), amoled.getName());
    }
    lv_task_handler();
    button.check();
}
