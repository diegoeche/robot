#include <sstream>
#include <iostream>

#include <string.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>

#include <stdarg.h>
#include <thread>
#include <mutex>

#include "data.h"
#include "helpers.h"
#include "robot.h"

extern "C" {
  #include <roboticscape.h>
}

void UDPLog(const char* format, ...) {
    char       msg[100];
    va_list    args;

    va_start(args, format);
    vsnprintf(msg, sizeof(msg), format, args); // do check return value
    va_end(args);

    printf(" [UDP Server] %s\n", msg);
}

void UDPServer() {
  struct sockaddr_in  si_me, si_other;
  unsigned int s, i, slen = sizeof(si_other), recv_len;
  char buf[BUFLEN];

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

    std::stringstream ss;
    ss << buf;
    std::string command;
    std::getline(ss, command);

    if(command == "UP") {
      sendCommand(UP);
      UDPLog("UP Command Sent");
    }

    if(command == "DOWN") {
      sendCommand(DOWN);
      UDPLog("DOWN Command Sent");
    }

    if(command == "NW") {
      sendCommand(NW);
      UDPLog("NW Command Sent");
    }

    if(command == "NE") {
      sendCommand(NE);
      UDPLog("NE Command Sent");
    }

    if(command == "LEFT") {
      sendCommand(LEFT);
      UDPLog("LEFT Command Sent");
    }

    if(command == "RIGHT") {
      sendCommand(RIGHT);
      UDPLog("RIGHT Command Sent");
    }

    if(command == "STOP") {
      sendCommand(STOP);
      UDPLog("STOP Command Sent");
    }

    if(command == "KILL") {
      sendCommand(KILL);
      UDPLog("KILL Command Sent");
    }

    if(command == "KILL") {
      sendCommand(KILL);
      UDPLog("KILL Command Sent");
    }

    if(command == "SET_CAMMERA") {
      float x;
      float y;
      ss >> x >> y;
      if(x > -1.5 && x < 1.5) rc_send_servo_pulse_normalized(7, x);
      if(y > -1.5 && y < 1.5) rc_send_servo_pulse_normalized(8, y);
      UDPLog("SET_CAMMERA");
    }
    // Reply the client with the same data
    /* if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1) die("sendto()"); */
  }
  printf(" * Closing Socket\n");
  close(s);
  printf(" * Ending 'UDPServer' Thread\n");
}
