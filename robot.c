#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <rc_usefulincludes.h>
#include <roboticscape.h>
#include <unistd.h>

#define BUFLEN 512  // UDP Server Buffer Size
#define PORT 8888   // The port on which to listen for incoming data

struct sockaddr_in si_me, si_other;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

int s, i, slen = sizeof(si_other), recv_len;
char buf[BUFLEN];

typedef enum robot_state_t{
  IDLE,
  GREEN_LED,
  RED_LED,
  UP,
  DOWN,
} robot_state_t;

robot_state_t command = IDLE;

void die(char *s) {
  perror(s);
  exit(1);
}

/* void sig_handler(int signo) */
/* { */
/*   printf("Here!"); */
/*   if (signo == SIGINT) { */
/*     printf("Here!"); */
/*     close(s); */
/*     rc_cleanup(); */
/*     die("Ctrl-C"); */
/*   } */
/* } */

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

  while(1) {
    printf("Waiting for data...\n");
    fflush(stdout);

    // Clean buffer before writing on it again.
    memset((char *) &buf, 0, sizeof(buf));

    // try to receive some data, this is a blocking call
    if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1) {
      die("recvfrom()");
    }

    printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
    printf("Data: %s\n", buf);

    if(strcmp(buf, "UP\n") == 0) {
      pthread_mutex_lock( &mutex1 );
      command = UP;
      pthread_mutex_unlock( &mutex1 );
      printf("Change to Up\n");
    }

    if(strcmp(buf, "STOP\n") == 0) {
      pthread_mutex_lock( &mutex1 );
      command = IDLE;
      pthread_mutex_unlock( &mutex1 );
      printf("Change to: Stop\n");
    }

    // Reply the client with the same data
    /* if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1) die("sendto()"); */
  }

  close(s);
}

void use_both_motors(float duty) {
  float speed_one = duty * 1.15;
  float speed_two = duty * 1.0;

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


void *Control() {
  float duty = 0.6;
  int time = 1;

  // sanity check cape library initialized
  if(rc_initialize()){
    printf("failed to initialize cape\n");
    return;
  }

  // bring H-bridges of of standby
  rc_enable_motors();

  rc_set_led(GREEN, ON);
  rc_set_led(RED, ON);

  printf("Starting motors\n\n");

  while(1) {
    rc_set_led(GREEN, ON);
    rc_set_led(RED, ON);

    pthread_mutex_lock( &mutex1 );
    robot_state_t read_command = command;
    pthread_mutex_unlock( &mutex1 );

    if(read_command == IDLE) {
      rc_set_motor_brake(1);
      rc_set_motor_brake(2);
    }

    if(read_command == UP) {
      use_both_motors(duty);
    }

    sleep(time);
  }
}

main() {
  int rc1, rc2;
  pthread_t thread1, thread2;

  /* if (signal(SIGINT, sig_handler) == SIG_ERR) printf("\ncan't catch SIGINT\n"); */

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
  printf("All Motors Off\n\n");

  // final cleanup
  rc_cleanup();

  exit(EXIT_SUCCESS);
}
