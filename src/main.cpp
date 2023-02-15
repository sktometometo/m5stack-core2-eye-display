#include <M5Core2.h>
#include <math.h>

#define LGFX_M5STACK_CORE2
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <LGFX_AUTODETECT.hpp>

#include "eye.hpp"

#include "ArduinoHardware.h"
#include "ros/node_handle.h"
#include "geometry_msgs/Point.h"

const int image_width = 139;
const int image_height = 120;
const char path_image_outline_right[] = "/eye_outline_right_resized.jpg";
const char path_image_pupil_right[] = "/eye_pupil_right_resized.jpg";
const char path_image_reflex_right[] = "/eye_reflex_right_resized.jpg";
const char path_image_outline_left[] = "/eye_outline_left_resized.jpg";
const char path_image_pupil_left[] = "/eye_pupil_left_resized.jpg";
const char path_image_reflex_left[] = "/eye_reflex_left_resized.jpg";

static Eye eye;

void callback(const geometry_msgs::Point& msg);

ros::NodeHandle_<ArduinoHardware> nh;
ros::Subscriber<geometry_msgs::Point> sub_point("~look_at", &callback);

bool mode_right;

float look_x = 0;
float look_y = 0;

void callback(const geometry_msgs::Point& msg)
{
  look_x = (float)msg.x;
  look_y = (float)msg.y;
}

void setup()
{
  M5.begin(true, false, true, true);
  SPIFFS.begin();

  nh.initNode();
  nh.subscribe(sub_point);

  while (not nh.connected())
  {
    nh.spinOnce();
    delay(1000);
  }

  if (not nh.getParam("~mode_right", &mode_right))
  {
    mode_right = true;
  }

  if (mode_right)
  {
    eye.init(path_image_outline_right, path_image_pupil_right, path_image_reflex_right, image_width, image_height);
  }
  else
  {
    eye.init(path_image_outline_left, path_image_pupil_left, path_image_reflex_left, image_width, image_height);
  }
  eye.update_look();

  nh.loginfo("Initialized.");
}

void loop()
{
  delay(100);
  nh.loginfo("update.");
  eye.update_look(look_x, look_y);
  nh.spinOnce();
}
