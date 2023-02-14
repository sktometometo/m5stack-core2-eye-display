#include <M5Core2.h>
#include <math.h>

#define LGFX_M5STACK_CORE2
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <LGFX_AUTODETECT.hpp>

class Eye
{
private:
  LGFX lcd;
  LGFX_Sprite sprite_eye;
  LGFX_Sprite sprite_outline;
  LGFX_Sprite sprite_pupil;
  LGFX_Sprite sprite_reflex;

  int zoom_ratio;

  int image_width;
  int image_height;

public:
  void init(const char* path_jpg_outline, const char* path_jpg_pupil, const char* path_jpg_reflex,
            const int image_width, const int image_height)
  {
    this->image_width = image_width;
    this->image_height = image_height;

    lcd.init();
    lcd.setRotation(0);
    if (lcd.width() < lcd.height())
    {
      lcd.setRotation(lcd.getRotation() ^ 1);
    }

    sprite_eye.createSprite(image_width, image_height);
    sprite_eye.fillScreen(TFT_WHITE);

    sprite_outline.createSprite(image_width, image_height);
    sprite_outline.fillScreen(TFT_WHITE);
    sprite_outline.drawJpgFile(SPIFFS, path_jpg_outline);
    sprite_pupil.createSprite(image_width, image_height);
    sprite_pupil.fillScreen(TFT_WHITE);
    sprite_pupil.drawJpgFile(SPIFFS, path_jpg_pupil);
    sprite_reflex.createSprite(image_width, image_height);
    sprite_reflex.fillScreen(TFT_WHITE);
    sprite_reflex.drawJpgFile(SPIFFS, path_jpg_reflex);

    zoom_ratio = (float)lcd.width() / image_width;
    float ztmp = (float)lcd.height() / image_height;
    if (zoom_ratio > ztmp)
    {
      zoom_ratio = ztmp;
    }

    lcd.setPivot(lcd.width() >> 1, lcd.height() >> 1);
    lcd.fillScreen(TFT_WHITE);
  }

  void update_look(float dx = 0.0, float dy = 0.0, float scale = 10.0, float random_scale = 5.0)
  {
    long rx = (int)(random_scale * random(100) / 100);
    long ry = (int)(random_scale * random(100) / 100);
    sprite_eye.fillScreen(TFT_WHITE);
    sprite_outline.pushSprite(&sprite_eye, 0, 0, TFT_WHITE);
    sprite_pupil.pushSprite(&sprite_eye, (int)(scale * dx), (int)(scale * dy), TFT_WHITE);
    sprite_reflex.pushSprite(&sprite_eye, (int)(scale * dx) + rx, (int)(scale * dy) + ry, TFT_WHITE);
    sprite_eye.pushRotateZoom(&lcd, lcd.width() >> 1, lcd.height() >> 1, 0, zoom_ratio, zoom_ratio, TFT_WHITE);
  }
};

class EyeMaster
{
public:
  EyeMaster()
  {
    Serial2.begin(115200, SERIAL_8N1, 14, 13);
  }

  size_t send_update_look_args(float dx = 0.0, float dy = 0.0, float scale = 10.0, float random_scale = 5.0)
  {
    float data[4];
    data[0] = dx;
    data[1] = dy;
    data[2] = scale;
    data[3] = random_scale;
    return Serial2.write((char*)data, sizeof(float) * 4);
  }
};

class EyeSlave
{
public:
  EyeSlave()
  {
    Serial2.begin(115200, SERIAL_8N1, 14, 13);
  }

  bool receive_update_look_args(float* dx, float* dy, float* scale, float* random_scale)
  {
    char buf[128];
    size_t size = Serial2.read(buf, 128);
    if (size == 4 * sizeof(float))
    {
      *dx = ((float*)buf)[0];
      *dy = ((float*)buf)[1];
      *scale = ((float*)buf)[2];
      *random_scale = ((float*)buf)[3];
      return true;
    }
    else
    {
      return false;
    }
  }

  bool available()
  {
    return Serial1.available() > 0;
  }
};
