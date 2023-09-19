#include <lvgl.h>
#include <TFT_eSPI.h>
#include <ui.h>

/*Don't forget to set Sketchbook location in File/Preferencesto the path of your UI project (the parent foder of this INO file)*/

/*Change to your screen resolution*/
static const uint16_t screenWidth = 320;
static const uint16_t screenHeight = 240;

static lv_disp_draw_buf_t draw_buf;
// static lv_color_t buf[ screenWidth * screenHeight / 10 ];
uint8_t *buf, *buf2;

uint8_t led1_stat, led2_stat;

TFT_eSPI tft = TFT_eSPI(screenHeight, screenWidth); /* TFT instance */

const uint8_t led1_pin = 15;
const uint8_t led2_pin = 2;
const uint8_t pot_pin = 34;
const uint8_t pwmPin = 0; 

// setting PWM properties
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;

uint32_t tt = 0, adcTime = 0, adcSum = 0;
uint16_t adcVal = 0, sampleCnt = 0;
uint8_t sliderVal = 0;
String tempStr;

#if LV_USE_LOG != 0
/* Serial debugging */
void my_print(const char *buf)
{
  Serial.printf(buf);
  Serial.flush();
}
#endif

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushColors((uint16_t *)&color_p->full, w * h, true);
  tft.endWrite();

  lv_disp_flush_ready(disp);
}

/*Read the touchpad*/
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
  uint16_t touchX = 0, touchY = 0;

  //  bool touched = false;//tft.getTouch( &touchX, &touchY, 600 );
  bool touched = tft.getTouch(&touchX, &touchY, 600);

  if (!touched)
  {
    data->state = LV_INDEV_STATE_REL;
  }
  else
  {
    data->state = LV_INDEV_STATE_PR;

    /*Set the coordinates*/
    data->point.x = touchX;
    data->point.y = touchY;

    //    Serial.print( "Data x " );
    //    Serial.print( touchX );
    //
    //    Serial.print("\t");
    //
    //    Serial.print( "Data y " );
    //    Serial.println( touchY );
  }
}

void task1(void *pvParameters)
{
  while (1)
  {
    // led durumlarını değiştir
    digitalWrite(led1_pin, led1_stat);
    digitalWrite(led2_pin, led2_stat);

    // her 10 milisaniyede 1 ADC' den 1 örnek oku ve okunan değerleri topla
    if (millis() - adcTime > 10)
    {
      adcSum += analogRead(pot_pin);
      sampleCnt++;
      adcTime = millis();
    }

    // her 100 milisaniyede bir
    // toplanan adc değerlerinin ortalamasını alıp ekranda göster
    // slider ın değerini oku
    // slider dan alınan değeri pwm olarak led e aktar
    if (millis() - tt > 100)
    {
      adcVal = adcSum / sampleCnt;
      sampleCnt = 0;
      adcVal = map(adcVal, 0, 4096, 0, 100);
      adcSum = 0;

      // ADC den okunan değeri progress bar a ve yanındaki text box a yazdır
      lv_bar_set_value(ui_pgLed1, adcVal, LV_ANIM_ON);
      lv_label_set_text(ui_txtPgVal1, (String("%") + String(adcVal)).c_str());

      Serial.printf("Slider Val :%d\n", sliderVal);

      // slider değerini oku ve yanındaki text box a yazdır
      sliderVal = lv_slider_get_value(ui_sliderLed3);
      lv_label_set_text(ui_txtSliderVal, (String("%") + String(sliderVal)).c_str());
      
      // slider dan okunan değeri 0 ile 255 arasına getirip led e aktar
      sliderVal = map(sliderVal, 0, 100, 0, 255);
      ledcWrite(ledChannel, sliderVal);

      tt = millis();
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void setup()
{
  Serial.begin(115200); /* prepare for possible serial debug */

  String LVGL_Arduino = "Hello Arduino! ";
  LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

  Serial.println(LVGL_Arduino);
  Serial.println("I am LVGL_Arduino");

  lv_init();

#if LV_USE_LOG != 0
  lv_log_register_print_cb(my_print); /* register print function for debugging */
#endif

  tft.begin();        /* TFT init */
  tft.setRotation(3); /* Landscape orientation, flipped */

  uint16_t calData[5] = {275, 3620, 264, 3532, 1};
  tft.setTouch(calData);

  buf = (uint8_t *)heap_caps_malloc(screenWidth * screenHeight / 10, MALLOC_CAP_DMA);
  buf2 = (uint8_t *)heap_caps_malloc(screenWidth * screenHeight / 10, MALLOC_CAP_DMA);
  lv_disp_draw_buf_init(&draw_buf, buf, buf2, screenWidth * screenHeight / 10);

  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  /*Change the following line to your display resolution*/
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  /*Initialize the (dummy) input device driver*/
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);

  ui_init();

  Serial.println("Setup done");

  pinMode(led1_pin, OUTPUT);
  pinMode(led2_pin, OUTPUT);
  pinMode(pot_pin, INPUT);
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(pwmPin, ledChannel);

  xTaskCreatePinnedToCore(
      task1,   // Function to implement the task
      "task1", // Name of the task
      1024,    // Stack size in words
      NULL,    // Task input parameter
      2,       // Priority of the task
      NULL,    // Task handle.
      0        // Core where the task should run
  );
}

void loop()
{
  lv_timer_handler(); /* let the GUI do its work */
  vTaskDelay(5 / portTICK_PERIOD_MS);
}
