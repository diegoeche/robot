#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <rc_usefulincludes.h>
#include <roboticscape.h>
#include <unistd.h>
#include <stdarg.h>

#define BUFLEN 512  // UDP Server Buffer Size
#define PORT 8888   // The port on which to listen for incoming data

struct sockaddr_in si_me, si_other;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

int s, i, slen = sizeof(si_other), recv_len;
char buf[BUFLEN];

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

robot_state_t command = STOP;

void die(char *s) {
  perror(s);
  exit(1);
}

void sig_handler(int signo)
{
  if (signo == SIGINT) {
    printf(" [Sig-Handler] SIGINT Received\n");
    close(s);
    rc_disable_motors();
    printf(" [Sig-Handler] All Motors Off\n");
    rc_cleanup();
    printf(" [Sig-Handler] Cleanup Done. Exiting\n");
    exit(0);
  }
}

void UDPLog(const char* format, ...) {
    char       msg[100];
    va_list    args;

    va_start(args, format);
    vsnprintf(msg, sizeof(msg), format, args); // do check return value
    va_end(args);

    printf(" [UDP Server] %s\n", msg);
}

void sendCommand(robot_state_t command) {
  pthread_mutex_lock( &mutex1 );
  command = command;
  pthread_mutex_unlock( &mutex1 );
}

void *UDPServer() {
  // Create a UDP socket
  if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
    die("socket");
  }

  // Zero out the structure
  memset((char *) &si_me, 0, sizeof(si_me));

  si_me.sin_family = AF_INET;
  si_me.sin_port = htons(PORT);
  si_me.sin_addr.s_addr = htonl(INADDR_ANY);

  // Bind to socket
  if(bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1) die("bind");

  while(1 && command != KILL) {
    fflush(stdout);

    // Clean buffer before writing on it again.
    memset((char *) &buf, 0, sizeof(buf));

    UDPLog("Waiting for data...");
    if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1) {
      die("recvfrom()");
    }

    UDPLog("Received packet from %s:%d", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
    UDPLog("Data: [%s  ]", buf);

    if(strcmp(buf, "UP\n") == 0) {
      sendCommand(UP);
      UDPLog("UP Command Sent");
    }

    if(strcmp(buf, "DOWN\n") == 0) {
      sendCommand(DOWN);
      UDPLog("DOWN Command Sent");
    }

    if(strcmp(buf, "NW\n") == 0) {
      sendCommand(NW);
      UDPLog("NW Command Sent");
    }

    if(strcmp(buf, "NE\n") == 0) {
      sendCommand(NE);
      UDPLog("NE Command Sent");
    }

    if(strcmp(buf, "LEFT\n") == 0) {
      sendCommand(LEFT);
      UDPLog("LEFT Command Sent");
    }

    if(strcmp(buf, "RIGHT\n") == 0) {
      sendCommand(RIGHT);
      UDPLog("RIGHT Command Sent");
    }

    if(strcmp(buf, "STOP\n") == 0) {
      sendCommand(STOP);
      UDPLog("STOP Command Sent");
    }

    if(strcmp(buf, "KILL\n") == 0) {
      sendCommand(KILL);
      UDPLog("KILL Command Sent");
    }

    // Reply the client with the same data
    /* if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1) die("sendto()"); */
  }
  printf(" * Closing Socket\n");
  close(s);
  printf(" * Ending 'UDPServer' Thread\n");
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

void *Control() {
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

  ControlLog("Motors activated. Starting Control Loop.");

  robot_state_t last_command = STARTING;

  while(1) {
    pthread_mutex_lock( &mutex1 );
    robot_state_t read_command = command;
    pthread_mutex_unlock( &mutex1 );
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

main() {
  int rc1, rc2;
  pthread_t thread1, thread2;

  if(rc1=pthread_create(&thread1, NULL, &UDPServer, NULL)) {
    printf("UDP thread failed: %d\n", rc1);
  }

  if( (rc2=pthread_create( &thread2, NULL, &Control, NULL)) ) {
    printf("Control Thread creation failed: %d\n", rc2);
  }

  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);

  // Disable motors
  rc_disable_motors();
  printf("All Motors Off\n");

  // final cleanup
  rc_cleanup();

  exit(EXIT_SUCCESS);
}
