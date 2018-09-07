#define BUFLEN 512  // UDP Server Buffer Size
#define PORT 8888   // The port on which to listen for incoming data
typedef enum robot_state_t{
  STARTING,
  STOP,
  NW, UP, NE,
  LEFT, RIGHT,
  DOWN,
  KILL,
  GREEN_LED,
  RED_LED,
} robot_state_t;

extern robot_state_t command;

#include <mutex>

extern std::mutex command_mutex;
