#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{

    char input[100];
    char *token1, *token2;
    int num1, num2;

    char *input_ptr = input;
    for (int i = 0; i < 100; i++) {
        int status = read(0, input_ptr, 1);
        if (status < 0) {
            fprintf(2, "Ошибка чтения файла.\n");
            exit(1);
        }

        if (*input_ptr == '\0' || *input_ptr == '\n' || *input_ptr == '\r' || status == 0)
            break;

        input_ptr++;
    }

    if (strchr(input, ' ') == input){
        token1 = strchr(input, ' ');
        token1++;
    }
    else {
        token1 = input;
    }
    if (!token1 ||( *token1 == '\n' || *token1 == '\r')) {
        fprintf(2, "Некорректный формат данных.\n");
        exit(1);
    }

    num1 = atoi(token1);

    token2 = strchr(token1, ' ');
    token2++;
    if (!token2 ||( *token2 == '\n' || *token2 == '\r')) {
        fprintf(2, "Некорректный формат данных. Отсутствует второе число.\n");
        exit(1);
    }


    num2 = atoi(token2);
    if (!num1 || !num2) {
        fprintf(2, "Некорректный формат данных.\n");
        exit(1);
    }
    int sum =  num1 + num2;
    printf("%d\n", sum);

    exit(0);
}
