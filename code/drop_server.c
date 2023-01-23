#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(){
  FILE* fd = fopen("config.txt", "r");
  char pid_[10];
  fgets(pid_, sizeof(pid_), fd);
  pid_t pid = strtol(pid_, NULL, 10);
  kill(pid, SIGTERM);
}