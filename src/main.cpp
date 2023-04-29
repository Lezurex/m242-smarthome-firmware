#include <Arduino.h>
#include <string>
#include <sstream>
#include <vector>
#include "view.h"
#include "networking.h"
#include "sideled.h"

void event_handler_button(struct _lv_obj_t *obj, lv_event_t event);
void init_gui_elements();
void mqtt_callback(char *topic, byte *payload, unsigned int length);

unsigned long next_lv_task = 0;

lv_obj_t *led;

lv_obj_t *red_checkbox;
lv_obj_t *blue_checkbox;
lv_obj_t *green_checkbox;

lv_obj_t *off_checkbox;
lv_obj_t *on_checkbox;
lv_obj_t *blink_checkbox;

lv_obj_t *wardrobe_button;
lv_obj_t *kitchen_button;
lv_obj_t *living_button;
lv_obj_t *bedroom_button;
lv_obj_t *hall_button;

std::string get_selected_color()
{
  std::string color = "";
  if (lv_checkbox_is_checked(red_checkbox))
  {
    color += "255,0,0";
  }
  else if (lv_checkbox_is_checked(blue_checkbox))
  {
    color += "0,0,255";
  }
  else if (lv_checkbox_is_checked(green_checkbox))
  {
    color += "0,255,0";
  }
  return color;
}

std::string get_selected_mode()
{
  std::string mode = "";
  if (lv_checkbox_is_checked(off_checkbox))
  {
    mode += "off";
  }
  else if (lv_checkbox_is_checked(on_checkbox))
  {
    mode += "on";
  }
  else if (lv_checkbox_is_checked(blink_checkbox))
  {
    mode += "flashing";
  }
  return mode;
}

void event_handler_button(struct _lv_obj_t *obj, lv_event_t event)
{
  if (event == LV_EVENT_CLICKED)
  {
    if (obj == living_button)
    {
      const std::string mode = get_selected_mode();
      mqtt_publish("smarthome/livingroom/mode", mode.c_str());
      mqtt_publish("smarthome/livingroom/color", get_selected_color().c_str());
      if (mode != "off")
      {
        mqtt_publish("smarthome/livingroom/brightness", "100");
      }
    }
    else if (obj == kitchen_button)
    {
      const std::string mode = get_selected_mode();
      mqtt_publish("smarthome/livingroom/mode", mode.c_str());
      mqtt_publish("smarthome/livingroom/color", get_selected_color().c_str());
      if (mode != "off")
      {
        mqtt_publish("smarthome/livingroom/brightness", "100");
      }
    }
    else if (obj == bedroom_button)
    {
      const std::string mode = get_selected_mode();
      mqtt_publish("smarthome/bedroom/mode", mode.c_str());
      mqtt_publish("smarthome/bedroom/color", get_selected_color().c_str());
      if (mode != "off")
      {
        mqtt_publish("smarthome/bedroom/brightness", "100");
      }
    }
    else if (obj == hall_button)
    {
      const std::string mode = get_selected_mode();
      mqtt_publish("smarthome/hallway/mode", mode.c_str());
      mqtt_publish("smarthome/hallway/color", get_selected_color().c_str());
      if (mode != "off")
      {
        mqtt_publish("smarthome/hallway/brightness", "100");
      }
    }
    else if (obj == wardrobe_button)
    {
      const std::string mode = get_selected_mode();
      mqtt_publish("smarthome/wardrobe/mode", mode.c_str());
      mqtt_publish("smarthome/wardrobe/color", get_selected_color().c_str());
      if (mode != "off")
      {
        mqtt_publish("smarthome/wardrobe/brightness", "100");
      }
    }
  }
}

void init_gui_elements()
{
  add_label("1. Select Color", 10, 10);
  red_checkbox = add_checkbox("Red", 10, 40);
  blue_checkbox = add_checkbox("Blue", 120, 40);
  green_checkbox = add_checkbox("Green", 230, 40);
  add_label("2. Select Mode", 10, 70);
  off_checkbox = add_checkbox("Off", 10, 100);
  on_checkbox = add_checkbox("On", 120, 100);
  blink_checkbox = add_checkbox("Blink", 200, 100);
  add_label("3. Select room:", 10, 140);
  living_button = add_button("Livingroom", event_handler_button, 0, 170, 75, 30);
  kitchen_button = add_button("Kitchen", event_handler_button, 85, 170, 75, 30);
  bedroom_button = add_button("Bedroom", event_handler_button, 170, 170, 75, 30);
  hall_button = add_button("Hallway", event_handler_button, 0, 210, 75, 30);
  wardrobe_button = add_button("Wardrobe", event_handler_button, 85, 210, 75, 30);
}

// ----------------------------------------------------------------------------
// MQTT callback
// ----------------------------------------------------------------------------

std::vector<std::string> decodeRgb(const std::string &s, char delimiter)
{
  std::vector<std::string> tokens;
  std::stringstream ss(s);
  std::string token;

  while (std::getline(ss, token, delimiter))
  {
    tokens.push_back(token);
  }

  return tokens;
}

void mqtt_callback(char *topic, byte *payload, unsigned int length)
{
  Serial.println("Message arrived " + String(topic));
  auto topicS = String(topic);
  String room = topicS.substring(0, topicS.lastIndexOf("/"));
  room = room.substring(room.lastIndexOf("/") + 1);

  // Parse Payload into String
  auto *buf = (char *)malloc((sizeof(char) * (length + 1)));
  memcpy(buf, payload, length);
  buf[length] = '\0';
  auto payloadS = String(buf);
  payloadS.trim();

  Serial.println(payloadS);

  uint8_t begin;
  uint8_t end;

  if (room == "wardrobe")
  {
    begin = 0;
    end = 6;
  }
  else if (room == "kitchen")
  {
    begin = 6;
    end = 12;
  }
  else if (room == "livingroom")
  {
    begin = 12;
    end = 19;
  }
  else if (room == "bedroom")
  {
    begin = 19;
    end = 24;
  }
  else if (room == "hallway")
  {
    begin = 24;
    end = 30;
  }
  if (topicS.endsWith("/mode"))
  {
    if (payloadS == "on")
    {
      set_led_state(begin, end, SIDELED_STATE_ON);
    }
    else if (payloadS == "off")
    {
      set_led_state(begin, end, SIDELED_STATE_OFF);
    }
    else if (payloadS == "flashing")
    {
      set_led_state(begin, end, SIDELED_STATE_BLINK);
    }
    else if (payloadS == "party")
    {
      set_led_state(begin, end, SIDELED_STATE_FADE);
    }
  }
  else if (topicS.endsWith("/color"))
  {
    auto rgb = decodeRgb(payloadS.c_str(), ',');
    CRGB color = CRGB(std::stoi(rgb[0]), std::stoi(rgb[1]), std::stoi(rgb[2]));
    set_led_color(begin, end, color);
  }
  else if (topicS.endsWith("/brightness"))
  {
    set_led_brightness(begin, end, std::stoi(payloadS.c_str()));
  }

  if (String(topic) == "example")
  {
    if (payloadS == "on")
    {
      lv_led_on(led);
    }
    if (payloadS == "off")
    {
      lv_led_off(led);
    }
  }
}

// ----------------------------------------------------------------------------
// UI event handlers
// ----------------------------------------------------------------------------

String buffer = "";

void event_handler_num(struct _lv_obj_t *obj, lv_event_t event)
{
}

lv_obj_t *mbox;

// ----------------------------------------------------------------------------
// MAIN LOOP
// ----------------------------------------------------------------------------

void loop()
{
  if (next_lv_task < millis())
  {
    lv_task_handler();
    next_lv_task = millis() + 5;
  }

  mqtt_loop();
}

// ----------------------------------------------------------------------------
// MAIN SETUP
// ----------------------------------------------------------------------------

void setup()
{
  init_m5();
  init_display();
  Serial.begin(115200);

  // Setup WiFi
  lv_obj_t *wifiConnectingBox = show_message_box_no_buttons("Connecting to WiFi...");
  lv_task_handler();
  delay(5);
  setup_wifi();

  Serial.println("WiFi connected");

  // Initialize MQTT
  mqtt_init(mqtt_callback);

  close_message_box(wifiConnectingBox);
  init_gui_elements();
  init_led();
}