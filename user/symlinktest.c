#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"
#include "kernel/stat.h"
#include "user/user.h"


char nbuffer[MAXPATH];
char buffer[13];
char const_buffer[13] = "Hello World!";

char* paths[] = {"/d1/d2/d3/abs", "/d1/d2/d3/aabs", "/d1/d2/d3/aaabs",
                 "/d1/d2/d3/na","/d1/d2/d3/nna", "/d1/d2/d3/nnna",
                 "na", "na1", "na2",
                 "rec", "rec2", "rec3", "rec1",
                 "ref", "cur", "up", "down"};
char* catalogs[] = {"/", "/d1", "/d1/d2",
                    "/d1/d2/d3", "/d1/d2/d3/d4", "/d1/d2/d3/d4/d5"};
void check_result(int result, char* mes) {
    if (result < 0) {
        fprintf(2, mes);
        exit(1);
    }
}


void makefile(char* filename) {
    int fd = open(filename, O_CREATE | O_WRONLY);
    check_result(fd, "Error creating file\n");
    check_result(write(fd, &const_buffer, sizeof(buffer)) - 1, "Error creating file\n");
    close(fd);
}

void makesymlink(char* targetpath, char* name) {
    check_result( symlink(targetpath, name), "Error creating symlink\n");
}

void compare(char* cur, char* best){
    if (strcmp(cur, best) != 0) {
        fprintf(2, "Error in symlink: %s %s\n", best, cur);
        exit(3);
    }
}

void check(char* targetpath, char* cur, int expected) {
    int fd = open(cur, O_RDONLY);
    if (fd < 0 && expected == 0) {
        return;
    }
    char current[sizeof(buffer)];
    char best[sizeof(buffer)];


    check_result(fd, "Error opening file1\n");
    check_result(expected - 1, "Error opening file2\n");
    check_result(read(fd, &current, sizeof(buffer)), "Error reading symlink\n");
    close(fd);
    fd = open(targetpath, O_RDONLY);
    check_result(fd, "Error opening file3\n");
    check_result(read(fd, &best, sizeof(buffer)) , "Error opening file4\n");
    close(fd);
    compare(current, best);
    printf("%s complete\n", cur);
}

void paths_check(char * file1){
    for (int i = 0; i < 17; ++i) {
        if (paths[i][0] == '/'){
            check(file1, paths[i], 1);

        } else if (paths[i][0] == 'n'){
            check_result(chdir("d1/d2/d3"), "Error in filestream\n");
            check("fst", paths[i], 1);
            check_result(chdir("../../.."), "Error in filestream\n");
        } else {
            check_result(chdir("d1/d2/d3"), "Error in filestream\n");
            check("", paths[i], 0);
            check_result(chdir("../../.."), "Error in filestream\n");
        }
    }
    printf("Tests passed\n");
}

void maketests(char *file1){
    // Создание символических ссылок для каждого из указанных случаев
    makesymlink(file1, "/d1/d2/d3/abs"); //Создание абсолютной символической ссылки

    // Создание цепочки символических ссылок
    makesymlink("d1/d2/d3/abs", "d1/d2/d3/aabs");
    makesymlink("d1/d2/d3/aabs", "d1/d2/d3/aaabs");

    makesymlink("d1/d2/d3/na", "d1/d2/d3/nna");
    makesymlink("d1/d2/d3/nna", "d1/d2/d3/nnna");
    // Создание цепочки символических ссылок с относительными путями
    check_result(chdir("d1/d2/d3"), "Error in filestream\n");
    makesymlink("./fst", "na");
    makesymlink("na", "na1");
    makesymlink("na1", "na2");
    makesymlink("../../snd", "n2");
    check("../../snd", "n2", 1);
    makesymlink("d4/d5/trd", "n3");
    check("d4/d5/trd", "n3", 1);

    // Создание некорректных символических ссылок(бесконечная рекурсия)
    makesymlink("rec", "rec");
    // Создание некорректных символических ссылок - косвенная ссылка на себя
    makesymlink("rec1", "rec2");
    makesymlink("rec2", "rec3");
    makesymlink("rec3", "rec1");
    // Создание некорректных символических ссылок - несуществующий файл
    makesymlink("uncor", "ref");
    // Создание некорректных символических ссылок - относительная ссылка на несуществующий файл того же каталога
    makesymlink("snd", "cur");
    // Создание некорректных символических ссылок - относительная ссылка на несуществующий файл каталога на 2-3 уровня выше
    makesymlink("../../fst", "up");
    // Создание некорректных символических ссылок - относительная ссылка на несуществующий файл каталога на 2-3 уровня ниже
    makesymlink("d4/d5/fst", "down");
    check_result(chdir("../../.."), "Error in filestream\n");
}

int main() {

// Создание необходимой структуры каталогов и файлов
    check_result(mkdir("d1"), "Error creating directory dir1\n");
    check_result(mkdir("d1/d2"), "Error creating directory dir2\n");
    check_result(mkdir("d1/d2/d3"), "Error creating directory dir3\n");
    check_result(mkdir("d1/d2/d3/d4"), "Error creating directory dir4\n");
    check_result(mkdir("d1/d2/d3/d4/d5"), "Error creating directory dir5\n");

    char * file1 = "d1/d2/d3/fst";
    makefile(file1);
    maketests(file1);
    paths_check(file1);

    return 0;
}