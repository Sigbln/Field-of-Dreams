#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

const char WORDS[6][50] = {"squirrel\0", "table\0", "snow\0", "sunflower\0", "director\0", "department\0"};

int RegisterListener(const char *hostname, const char *port_) {
  int listen_fd = socket(AF_INET, SOCK_STREAM, 0);

  uint16_t port = htons(strtol(port_, NULL, 10));
  in_addr_t ip = inet_addr(hostname);

  struct sockaddr_in serv_addr = {
      .sin_family = AF_INET,
      .sin_port = port,
      .sin_addr.s_addr = ip
  };

  if (bind(listen_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1) {
    perror("ERROR: bind");
    exit(1);
  }

  if (listen(listen_fd, 1) == -1) {
    perror("ERROR: listen");
    exit(1);
  }

  return listen_fd;
}

// Проверяем наличие буквы в слове и следим за неудачными попытками
void CheckLater(const char *word, char *mask, const char *get, int *attempts) {
  if (memchr(word, *get, strlen(word)) == NULL) {
    --(*attempts);
  } else {
    for (int i = 0; i < strlen(word); ++i) {
      if (*get == word[i]) {
        mask[i] = *get;
      }
    }
  }
}

int main(int argc, char *argv[]) {
  FILE* fd = fopen("config.txt", "w");
  fprintf(fd, "%d", getpid());
  fclose(fd);

  if (argc < 3) {
    perror("Invalid count of arguments.\n");
    return 1;
  }

  const char* host = argv[1];
  const char* port = argv[2];

  srand(time(NULL));
  int listen_fd = RegisterListener(host, port);

//  блокируем все возможные сигналы, чтобы сервер не прерывал работу
  sigset_t block;
  sigemptyset(&block);
  sigaddset(&block, SIGPIPE);
  sigprocmask(SIG_BLOCK, &block, NULL);

  while (1) {
    int connfd = accept(listen_fd, (struct sockaddr *) NULL, NULL);

//  готовим начальную маску
    const char *word = WORDS[rand() % 6];
    int word_len = strlen(word);
    char mask[50];
    memset(mask, '*', word_len);
    mask[word_len] = '\0';

    int attempts = 10; // Кол-во попыток

    while (attempts) {
//      Отправляем ответ клиенту
      if (write(connfd, mask, sizeof(mask)) < 0) {
        perror("ERROR: write");
        break;
      }
//      Читаем ответ клиента
      char get;
      if (read(connfd, &get, sizeof(get)) < 0) {
        perror("ERROR: read");
        break;
      }
//      Проверяем букву
      CheckLater(word, mask, &get, &attempts);
    }

    shutdown(connfd, SHUT_RDWR);
    close(connfd);
  }
}

