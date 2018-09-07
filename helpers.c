#include <thread>
#include <unistd.h>

void die(char *s) {
  perror(s);
  exit(1);
}
