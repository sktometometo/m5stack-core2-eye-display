#include <M5Core2.h>
#include <math.h>

#define LGFX_M5STACK_CORE2
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <LGFX_AUTODETECT.hpp>

#include "eye.hpp"

#define RIGHTEYE

const int image_width = 139;
const int image_height = 120;
const char path_image_outline_right[] = "/eye_outline_right_resized.jpg";
const char path_image_pupil_right[] = "/eye_pupil_right_resized.jpg";
const char path_image_reflex_right[] = "/eye_reflex_right_resized.jpg";
const char path_image_outline_left[] = "/eye_outline_left_resized.jpg";
const char path_image_pupil_left[] = "/eye_pupil_left_resized.jpg";
const char path_image_reflex_left[] = "/eye_reflex_left_resized.jpg";

static std::uint32_t count = 0;

static Eye eye;
#ifdef RIGHTEYE
static EyeMaster eye_master;
#else
static EyeSlave eye_slave;
#endif

void setup()
{
  M5.begin(true, false, true, true);
  SPIFFS.begin();
#ifdef RIGHTEYE
  eye.init(path_image_outline_right, path_image_pupil_right, path_image_reflex_right, image_width, image_height);
#else
  eye.init(path_image_outline_left, path_image_pupil_left, path_image_reflex_left, image_width, image_height);
#endif
  eye.update_look();

  Serial.println("Initialized.");
}

void loop()
{
#ifdef RIGHTEYE
  delay(100);
  float dx = cos(2 * M_PI * count / 20);
  float dy = sin(2 * M_PI * count / 20);
  eye.update_look(dx, dy);
  auto ret = eye_master.send_update_look_args(dx, dy);
  Serial.printf("Send %d bytes\n", ret);
  count++;
#else
  if (eye_slave.available())
  {
    float dx = 0;
    float dy = 0;
    float scale;
    float random_scale;
    if (eye_slave.receive_update_look_args(&dx, &dy, &scale, &random_scale))
    {
      eye.update_look(dx, dy);
    }
    else
    {
      Serial.println("Data receive failed.");
    }
  }
  else
  {
    Serial.println("No data available");
    delay(100);
  }
#endif
}
