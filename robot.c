#include <sstream>
#include <iostream>

#include <string.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>

extern "C" {
  #include <roboticscape.h>
}

#include <stdarg.h>
#include <thread>
#include <mutex>

#include "data.h"
#include "udp_server.h"
#include "helpers.h"

extern robot_state_t command = STOP;
std::mutex command_mutex;

/* extern "C" void rc_disable_motors(); */
void sendCommand(robot_state_t comm) {
  std::lock_guard<std::mutex> guard(command_mutex);
  command = comm;
}

void sig_handler(int signo) {
  if (signo == SIGINT) {
    printf(" [Sig-Handler] SIGINT Received\n");
    /* close(s); */
    rc_disable_motors();
    rc_disable_servo_power_rail();
    printf(" [Sig-Handler] All Motors Off\n");
    rc_cleanup();
    printf(" [Sig-Handler] Cleanup Done. Exiting\n");
    exit(0);
  }
}


void StatsServer() {
  struct sockaddr_in so_other;

  int s2, i2, slen2=sizeof(so_other);

  // Create a UDP socket
  if ((s2=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
    die("socket");
  }

  // Zero out the structure
  memset((char *) &so_other, 0, sizeof(so_other));

  so_other.sin_family = AF_INET;
  so_other.sin_port = htons(8090);

  if (inet_aton("10.171.31.122", &so_other.sin_addr)==0) {
    fprintf(stderr, "inet_aton() failed\n");
    exit(1);
  }

  char myBuffer[BUFLEN];
  while (1) {
    printf("Sending packet.\n");
    sprintf(myBuffer, "I'm Alive!\n");
    if (sendto(s2, myBuffer, BUFLEN, 0, (const sockaddr*) &so_other, slen2)==-1) die("sendto()");
    usleep(5 * 1000 * 1000);
  }

  close(s2);
}



void use_both_motors(float left_duty, float right_duty) {
  float speed_one = left_duty * 1.15;
  float speed_two = right_duty * 1.0;

  if (speed_one > 1.0) {
    speed_one = 1.0;
  }
  if (speed_two > 1.0) {
    speed_two = 1.0;
  }
  if (speed_one < -1.0) {
    speed_one = -1.0;
  }
  if (speed_two < -1.0) {
    speed_two = -1.0;
  }

  rc_set_motor(1, speed_one);
  rc_set_motor(2, speed_two);
}

void ControlLog(const char* format, ...) {
    char       msg[100];
    va_list    args;

    va_start(args, format);
    vsnprintf(msg, sizeof(msg), format, args); // do check return value
    va_end(args);

    printf(" [Control] %s\n", msg);
}

void Control() {
  float duty = 0.6;
  int time = 1;

  // Sanity check cape library initialized
  if(rc_initialize()){
    printf("Failed to initialize cape\n");
    return;
  }

  // Disable the RC's Signal Handler. We'll implement our own
  rc_disable_signal_handler();
  if (signal(SIGINT, sig_handler) == SIG_ERR) printf("\nCan't catch SIGINT\n");

  // Bring H-bridges of standby
  rc_enable_motors();
  rc_enable_servo_power_rail();

  ControlLog("Motors activated. Starting Control Loop.");

  robot_state_t last_command = STARTING;

  while(1) {
    std::lock_guard<std::mutex> guard(command_mutex);
    robot_state_t read_command = command;

    if(read_command != last_command) {
      switch(read_command) {
      case STOP:
	rc_set_motor_brake(1);
	rc_set_motor_brake(2);
	ControlLog("Brakes 1,2 activated");
	break;
      case UP:
	use_both_motors(duty, duty);
	ControlLog("Going forward at: [%0.4f]", duty);
	break;
      case LEFT:
	use_both_motors(duty, duty * 0.5f);
	ControlLog("Going left at: [%0.4f]", duty);
	break;
      case RIGHT:
	use_both_motors(duty * 0.5f, duty);
	ControlLog("Going right at: [%0.4f]", duty);
	break;
      case NW:
	use_both_motors(duty, duty * 0.75f);
	ControlLog("Going nw at: [%0.4f]", duty);
	break;
      case NE:
	use_both_motors(duty * 0.75f, duty);
	ControlLog("Going ne at: [%0.4f]", duty);
	break;
      case DOWN:
	use_both_motors(-duty, -duty);
	ControlLog("Going backwards at: [%0.4f]", duty);
	break;
      case KILL:
	ControlLog("Ending 'Control' Thread");
	return;
      default:
	ControlLog("Message not implemented");
      }
      last_command = read_command;
    }
    usleep(50 * 1000);
  }
}

int main() {
  int rc1, rc2;
  std::thread t1(UDPServer);
  std::thread t2(StatsServer);
  std::thread t3(Control);

  t1.join();
  t2.join();
  t3.join();

  // Disable motors
  rc_disable_motors();
  rc_disable_servo_power_rail();
  printf("All Motors Off\n");

  // final cleanup
  rc_cleanup();

  exit(EXIT_SUCCESS);
}
