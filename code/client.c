#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int ConnectToServer(const char *hostname, const char *port_) {
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

  uint16_t port = htons(strtol(port_, NULL, 10));
  in_addr_t ip = inet_addr(hostname);

  struct sockaddr_in ipv4_addr = {
      .sin_family = AF_INET,
      .sin_port = port,
      .sin_addr = ip
  };

  int is_connected = -1;
  while (is_connected == -1) {
    printf("Connecting to server...\n");
    is_connected = connect(socket_fd, (struct sockaddr *) &ipv4_addr, sizeof(ipv4_addr));
    sleep(1);
  }
  printf("Connected!\n");
  return socket_fd;
}

void Game(int client_socket){
  char secret_word[50];
  char symbol;

  while (1){
//    Читаем ответ сервера
    if(read(client_socket, secret_word, sizeof(secret_word))<0){
      perror("ERROR: read");
      exit(1);
    }

    //    Проверяем есть ли ещё в слове не разгаданные буквы
    if(memchr(secret_word, '*', strlen(secret_word))==NULL){
      printf("\nVictory, you guessed the word: %s\n", secret_word);
      break;
    }

    printf("\nYou need to guess the word: %s\nEnter any English letter: ", secret_word);
    scanf("%c", &symbol);
    while(getchar()!='\n'); // Чистим поток ввода от лишних символов

    //    Отправляем ответ серверу
    if(write(client_socket, &symbol, sizeof(symbol))<0){
      printf("\nOops, you probably lost(\n");
      exit(0);
    }
  }
}

int main(int argc, char **argv) {
  if (argc < 3) {
    perror("Invalid count of arguments.\n");
    return 1;
  }

  //  блокируем SIGPIPE
  sigset_t block;
  sigemptyset(&block);
  sigaddset(&block, SIGPIPE);
  sigprocmask(SIG_BLOCK, &block, NULL);

  const char *host = argv[1];
  const char *port = argv[2];

  int client_socket = ConnectToServer(host, port);

  Game(client_socket);

  shutdown(client_socket, SHUT_RDWR);
  close(client_socket);

  return 0;
}

