// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.2
// LVGL version: 8.2.0
// Project name: test1_project

#ifndef _TEST1_PROJECT_UI_H
#define _TEST1_PROJECT_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined __has_include
  #if __has_include("lvgl.h")
    #include "lvgl.h"
  #elif __has_include("lvgl/lvgl.h")
    #include "lvgl/lvgl.h"
  #else
    #include "lvgl.h"
  #endif
#else
  #include "lvgl.h"
#endif

#include "ui_helpers.h"
#include "ui_events.h"
// SCREEN: ui_Screen1
void ui_Screen1_screen_init(void);
extern lv_obj_t *ui_Screen1;
void ui_event_btnPage1( lv_event_t * e);
extern lv_obj_t *ui_btnPage1;
extern lv_obj_t *ui_Label6;
void ui_event_btnPage2( lv_event_t * e);
extern lv_obj_t *ui_btnPage2;
extern lv_obj_t *ui_Label2;
// SCREEN: ui_Screen2
void ui_Screen2_screen_init(void);
extern lv_obj_t *ui_Screen2;
extern lv_obj_t *ui_Panel1;
extern lv_obj_t *ui_Label3;
void ui_event_swLed1( lv_event_t * e);
extern lv_obj_t *ui_swLed1;
void ui_event_btnMain1( lv_event_t * e);
extern lv_obj_t *ui_btnMain1;
extern lv_obj_t *ui_Label5;
extern lv_obj_t *ui_Panel2;
extern lv_obj_t *ui_Label4;
void ui_event_swLed2( lv_event_t * e);
extern lv_obj_t *ui_swLed2;
// SCREEN: ui_Screen3
void ui_Screen3_screen_init(void);
extern lv_obj_t *ui_Screen3;
extern lv_obj_t *ui_Panel3;
extern lv_obj_t *ui_sliderLed3;
extern lv_obj_t *ui_txtSliderVal;
void ui_event_btnMain2( lv_event_t * e);
extern lv_obj_t *ui_btnMain2;
extern lv_obj_t *ui_label20;
extern lv_obj_t *ui_Panel4;
extern lv_obj_t *ui_txtPgVal1;
extern lv_obj_t *ui_pgLed1;
extern lv_obj_t *ui_Label1;
extern lv_obj_t *ui_Label7;
extern lv_obj_t *ui____initial_actions0;




void ui_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
