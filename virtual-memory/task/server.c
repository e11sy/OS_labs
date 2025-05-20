#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define FILENAME "shared_file"
#define FILESIZE 4096

int main() {
  int file = -1;
  void *ptr = NULL;
  int running = 1;

  while (running) {
    printf("\n--- Меню Сервера ---\n");
    printf("1. Выполнить проецирование\n");
    printf("2. Записать данные\n");
    printf("3. Завершить работу\n");
    printf("Выберите пункт: ");

    int choice;
    scanf("%d", &choice);
    getchar();

    switch (choice) {
      case 1:
        if (file == -1) {
          // file = open("shared_file", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
          file = open("shared_file", O_CREAT | O_RDWR, 0666);
          if (file == -1) {
            perror("open");
            exit(EXIT_FAILURE);
          }
          if (ftruncate(file, FILESIZE) == -1) {
            perror("ftruncate");
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
          printf("Введите строку для записи: ");
          char buffer[FILESIZE];
          fgets(buffer, sizeof(buffer), stdin);
          snprintf(ptr, FILESIZE, "%s", buffer);
          printf("Данные записаны.\n");
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
        unlink(FILENAME);
        printf("Файл удален.\n");
        break;

      default:
        printf("Неверный выбор.\n");
        break;
    }
  }

  return 0;
}
