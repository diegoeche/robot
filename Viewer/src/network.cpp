#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#define DEBUG 0
#define PORT 8887

struct sockaddr_in si_me, si_other;

int s;
int s_len, recv_len;
socklen_t slen = sizeof(si_other);

void die(const char error[]) {
  perror(error);
  exit(1);
}

void networkInitialize(const char server[]) {
  // Create a UDP socket
  s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (s == -1){
    die("socket");
  }

  // Zero out the structure
  memset((char *) &si_me, 0, sizeof(si_me));

  si_me.sin_family = AF_INET;
  si_me.sin_port = htons(8887);
  si_me.sin_addr.s_addr = htonl(INADDR_ANY);


  si_me.sin_family = AF_INET;
  inet_aton(server , &si_other.sin_addr);
  si_other.sin_port = htons(8888);
}

void sendCommand(const char message[]) {
  if (sendto(s, message, strlen(message) , 0 , (struct sockaddr *) &si_other, slen)==-1){
    die("sendto()");
  }
}
