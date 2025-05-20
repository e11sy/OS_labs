#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <unistd.h>

#define FILENAME "shared_file"
#define FILESIZE 4096

int main() {
  int file = -1;
  char *ptr = NULL;
  int running = 1;

  while (running) {
    printf("\n--- Меню Клиента ---\n");
    printf("1. Выполнить проецирование\n");
    printf("2. Прочитать данные\n");
    printf("3. Завершить работу\n");
    printf("Выберите пункт: ");

    int choice;
    scanf("%d", &choice);
    getchar();

    switch (choice) {
      case 1:
        if (file == -1) {
          file = open(FILENAME, O_RDWR, 0666);
          if (file == -1) {
            perror("open");
            exit(EXIT_FAILURE);
          }
          ptr = mmap(NULL, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, file, 0);
          if (ptr == MAP_FAILED) {
            perror("mmap");
            exit(EXIT_FAILURE);
          }
          printf("Проецирование выполнено.\n");
        } else {
          printf("Проецирование уже выполнено.\n");
        }
        break;

      case 2:
        if (ptr != NULL) {
          fd_set read_files;
          struct timeval timeout;

          FD_ZERO(&read_files);
          FD_SET(file, &read_files);

          timeout.tv_sec = 5;
          timeout.tv_usec = 0;

          printf("Ожидание данных...\n");
          int result = select(file + 1, &read_files, NULL, NULL, &timeout);
          if (result > 0) {
            printf("Считанные данные: %s\n", ptr);
          } else if (result == 0) {
            printf("Таймаут ожидания.\n");
          } else {
            perror("select");
          }
        } else {
          printf("Сначала выполните проецирование.\n");
        }
        break;

      case 3:
        running = 0;
        if (ptr != NULL) {
          munmap(ptr, FILESIZE);
          printf("Проецирование отменено.\n");
        }
        if (file != -1) {
          close(file);
          printf("Файл закрыт.\n");
        }
        break;

      default:
        printf("Неверный выбор.\n");
        break;
    }
  }

  return 0;
}
