#include "esphome.h"

#include <json_util.h>

class MyCustomUART : public Component, public UARTDevice {
  Sensor *xsensor1 {nullptr};

 public:
  MyCustomUART(UARTComponent *parent, Sensor *sensor1) : UARTDevice(parent) , xsensor1(sensor1) {}


  void setup() override {
    // nothing to do here
  }

  int readline(int readch, char *buffer, int len)
  {
    static int pos = 0;
    int rpos;

    if (readch > 0) {
      switch (readch) {
        case '\n': // Ignore new-lines
          break;
        case '\r': // Return on CR
          rpos = pos;
          pos = 0;  // Reset position index ready for next time
          return rpos;
        default:
          if (pos < len-1) {
            buffer[pos++] = readch;
            buffer[pos] = 0;
          }
      }
    }
    // No end of line has been found, so return -1.
    return -1;
  }

  void loop() override {
    const int max_line_length = 3000;
    static char buffer[max_line_length];
    while (available()) {
      if(readline(read(), buffer, max_line_length) > 0) {
        // publish_state(buffer);
        DynamicJsonBuffer doc(3000); //cannot be smaller, response is big!
        JsonObject& root = doc.parseObject(buffer);
        //publish_state(root["energy_wh"].as<char*>());
        if (xsensor1 != nullptr)   xsensor1->publish_state(root["real_power_watt"]);
      }
    }
  }
};
