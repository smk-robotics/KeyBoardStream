#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <linux/input.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>

const char *deviceInputAdress = "/dev/input/event15"; //// Путь до файла с событиями нужного устройства.
char deviceName[256] = "Unknown"; //// Переменная для имени устройства, события которого будут считываться.

int main (int argc, char *argv[])
{
    struct input_event inputEvent[64];
    int fd, rd, size = sizeof (struct input_event);

    printf("\033[32mKeyBoardStream -> ACTIVATED!\033[0m \n");

    //// ПЕРЕВОД ТЕРМИНАЛА В RAW РЕЖИМ ////
    // Структура для хранения старых настроек терминала (до изменений).
    struct termios orig_term_attr;
    // Структура для хранения новых настроек терминала (после изменений).
    struct termios new_term_attr;
    // Сохранение старых настроек терминала.
    tcgetattr(fileno(stdin), &orig_term_attr);
    // Копирование старых настроек в новые
    memcpy(&new_term_attr, &orig_term_attr, sizeof(struct termios));
    // Изменение настроек терминала. Отключен вывод символов в консоль.
    new_term_attr.c_lflag &= ~(ECHO|ICANON);
    new_term_attr.c_cc[VTIME] = 0;
    new_term_attr.c_cc[VMIN] = 0;
    // Применение новых настроек для терминала.
    tcsetattr(fileno(stdin), TCSANOW, &new_term_attr);

    //// РАЗЛИЧНЫЕ ПРОВЕРКИ /////
    // Проверка открытия файла с событиями устройства (клавиатуры).
    if ((fd = open(deviceInputAdress, O_RDONLY)) == -1)
    {
        printf ("\033[31mERROR! -> Can not get acess to %s\033[0m\n", deviceInputAdress);
        // Возврат старых настроек для терминала
        tcsetattr(fileno(stdin), TCSANOW, &orig_term_attr);
        return -1;
    }

    // Проверка запуска программы под root'ом.
    if ((getuid ()) != 0)
    {
        printf ("\033[31mERROR! -> You are not root! This may not work!\033[0m\n");
        // Возврат старых настроек для терминала
        tcsetattr(fileno(stdin), TCSANOW, &orig_term_attr);
        return -1;
    }

    ioctl (fd, EVIOCGNAME (sizeof(deviceName)), deviceName);
    printf ("Reading from: %s (%s) \n", deviceInputAdress, deviceName);

    //// ОСНОВНОЙ ЦИКЛ СЧИТЫВАНИЯ СОБЫТИЙ КЛАВИАТУРЫ ////
    while (1)
    {
        // Считывание буфера событий клавиатуры.
        if ((rd = read(fd, inputEvent, size * 64)) < size)
            printf ("\033[31mERROR! -> Can't read keyboard input event!\033[0m\n");
        // Выводим на экран код кнопки, которая была нажата.

        if (inputEvent[1].value == 1 && inputEvent[1].type == 1)
        {
            printf ("Code pressed[%d] \n", (inputEvent[1].code));
            // Если была нажата клавиша ESC - выходим из цикла.
            if (inputEvent[1].code == 1)
                break;
        }

        // Выводим на экран код кнопки, которая была отпущена.
        if (inputEvent[1].value == 0 && inputEvent[1].type == 1)
            printf ("Code released[%d] \n", (inputEvent[1].code));
    }
    // Закрытия файлового дескриптора буфера событий клавиатуры
    close(fd);
    // Возврат старых настроек для терминала
    tcsetattr(fileno(stdin), TCSANOW, &orig_term_attr);

    printf("\033[32mKeyBoardStream -> EXIT!\033[0m\n");

    return 0;
}
