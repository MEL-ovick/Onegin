#include <stdio.h>
#include <errno.h>       //perror
#include <sys/stat.h>   //stat
#include <assert.h>      //assert
#include <stdlib.h>
#include <fcntl.h>        //O_RDONLY
#include <unistd.h>       //open
#include <ctype.h>       //isalpha
#include <stdbool.h>     //bool буль-буль
#include "header.h"
#include "Sort.c"

#define FILE_NAME "onegin.txt"
#define SORTED_FILE "SortedOnegin.txt"
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

typedef struct {
     int i_a, i_b, delta;
} ComparisonStart;

enum string_order {b_before_a = -1, a_equivalent_b, a_before_b};

const long long MAX_BUFFER_SIZE = 64*1024;
const long long MAX_FILE_SIZE = 1024*1024;

int          ArgErrorCheck                   (int argc, char *argv[], char **file_name, char **sorted_file);
long long GetStatFileSize                  (const char* file_name);
FILE      *FopenError                        (const char* file_name);
bool        SetBuffer                           (long long file_size, FILE *fp);
char       *ReadFileDistributor           (const char *file_name, long long *n);
char       *ReadFileViaFopen               (const char *file_name, long long *n, long long file_size);
char       *ReadFileViaOpen                (const char *file_name, long long *n, long long file_size);
bool        FreadError                         (FILE *fp, long long *n, long long file_size, char *buf);
bool        ReadError                           (int fd, long long *n, long long file_size, char *buf);
void        CleanCRLF                           (long long *n, char *buf);
long long CountRows                           (const long long size, const char *buf);
char       **CreareIndexBox              (const char *buf, const long long size, long long *k);
void        PrintBuffer                         (char **index_box, const long long size_index_box);
long long AssignPointers                    (const char *buf, const long long size, char **index_box, long long *k);
int          CompareDownFileFromBegin(const void *str_a, const void *str_b);
int          CompareDownFileFromEnd  (const void *str_a, const void *str_b);
int          CompareStr                        (const char *a, const char *b,
                                                                    ComparisonStart (*WhereStartCompare)(size_t, size_t),
                                                                    int (*CompareFunc)(const char, const char));
size_t     FindStrLen                        (const char *row);
void        SkipToLetter                     (const char *a, const char *b,ComparisonStart *u, size_t len_a, size_t len_b);
int          CompareDownChar              (const char a, const char b);
ComparisonStart BeginStartCompare(size_t len_a, size_t len_b);
ComparisonStart EndStartCompare  (size_t len_a, size_t len_b);
int          CreateOutputDataFile         (const char *file_name);
int          WriteFileViaOpen               (int fd, const char *buf, const long long size);
FILE      *FCreateOutputDataFile      (const char *file_name);
int          CompareDownPointers         (const void *value_a, const void *value_b);
int          WriteFileViaFopen               (FILE *fp, char **index_box, const long long size_index_box);
FILE      *FCreateOutputDataFile       (const char *file_name);
void        End                                     (FILE *fp, char **index_box, char *buf);

int main(int argc, char *argv[]) {
    char *file_name = NULL, *sorted_file = NULL;
    ArgErrorCheck(argc, argv, &file_name, &sorted_file);

    long long size = 0, size_index_box = 0;

    char *buf = ReadFileDistributor(file_name, &size);
    if (!buf) return 1;

    char **index_box = CreareIndexBox(buf, size, &size_index_box);
    if (!index_box) return 1;

    FILE *fp = FCreateOutputDataFile (sorted_file);
    if (!fp) return 1;

    qSort((void *)index_box, size_index_box, sizeof(index_box[0]), &CompareDownFileFromBegin);
    int q = WriteFileViaFopen(fp, index_box, size_index_box);
    if (q != 0) {End(fp, index_box, buf); return 1;}

    qSort((void *)index_box, size_index_box, sizeof(index_box[0]), &CompareDownFileFromEnd);
    q = WriteFileViaFopen(fp, index_box, size_index_box);
    if (q != 0) {End(fp, index_box, buf); return 1;}

    qSort((void *)index_box, size_index_box, sizeof(index_box[0]), &CompareDownPointers);
    q = WriteFileViaFopen(fp, index_box, size_index_box);
    if (q != 0) {End(fp, index_box, buf); return 1;}

    End(fp, index_box, buf);
    return 0;
}

void End(FILE *fp, char **index_box, char *buf) {
    if (fp != NULL) {
        if (fclose(fp) == EOF) perror("fclose");
    }
    free(index_box);
    free(buf);
}

char *ReadFileDistributor(const char *file_name, long long *n) {
    long long file_size = GetStatFileSize(file_name);
    if (file_size <= 0) {return NULL;}

    char *buf = NULL;
    if (file_size < MAX_BUFFER_SIZE) {
        buf = ReadFileViaFopen(file_name, n, file_size);
    }
    else if (file_size < MAX_FILE_SIZE) {
        buf = ReadFileViaOpen(file_name, n, file_size);
    }
    else {printf("Слишком большой файл. Попробуй обрабатывать по кусочкам или обращаться напрямую через map");}
    return buf;
}

char *ReadFileViaFopen(const char *file_name, long long *n, long long file_size) {
    FILE *fp = FopenError(file_name);
    if (fp == NULL) {return NULL;}

    if (!SetBuffer(file_size, fp)) {fclose(fp);return NULL;}
    char *buf = (char*)calloc(file_size+1, 1);
    if (!buf) { perror("calloc"); fclose(fp); return NULL; }

    if (!FreadError(fp, n, file_size, buf)) return NULL;
    fclose(fp);
    return buf;
}

char *ReadFileViaOpen(const char *file_name, long long *n, long long file_size) {
    int fd = open(file_name, O_RDONLY);
    if (fd == -1) {perror("open failed");return NULL;}

    char *buf = (char*)calloc(file_size+1, 1);
    if (!buf) {perror("calloc"); close(fd); return NULL;}

    if (!ReadError(fd, n, file_size, buf)) return NULL;
    close(fd);
    CleanCRLF(n, buf);
    return buf;
}

long long GetStatFileSize(const char *file_name) {
    assert(file_name != NULL);

    struct stat st;
    if (stat(file_name, &st) == -1) {
        perror("stat");
        return 0;
    }
    if (S_ISREG(st.st_mode)) {
        long long int size = (long long)st.st_size;
        printf("Это обычный файл, размер: %lld байт\n", size);
        return size;
    }
    else {
        printf(CLRERROR("Не удалось обработать файл"));
        return 0;
    }
}

int ArgErrorCheck(int argc, char *argv[], char **file_name, char **sorted_file) {
    assert (argv != NULL);

    if (argc != 3) {
        printf(CLRERROR("Использование: %s имя_считаемого_файла имя_файла_для_записи\n"), argv[0]);
        return 1;
    }
    (*file_name) = argv[1];
    assert (file_name != NULL);
    (*sorted_file) = argv[2];
    assert (sorted_file != NULL);
    return 0;
}

FILE * FopenError(const char* file_name) {
    assert (file_name != NULL);
    FILE * fp = NULL;

     if ( ( fp = fopen (file_name, "r")) == NULL) {
        printf(CLRERROR("Не удается открыть %s\n"), file_name);
        return NULL;
    }
    return fp;
}

bool FreadError(FILE *fp, long long *n, long long file_size, char *buf) {
    (*n) = fread(buf, sizeof(buf[0]), (size_t)file_size, fp);

    if ((*n) < file_size) {
        if (ferror(fp)) {perror("fread");return false;} // Реальная ошибка чтения
        else if (feof(fp)) printf("Прочитано %zu элементов (конец файла)\n", (*n)); // Нормально: дошли до конца файла
    }
    buf[(*n)] = '\0';
    return true;
}

bool ReadError(int fd, long long *n, long long file_size, char *buf) {

    (*n) = read(fd, buf, (size_t)file_size);   // файл -o buf, напрямую
    if ((*n) == -1) {perror("read");return false;}

    else if ((*n) < file_size) {

        if ((*n) == 0) {printf("Пустой файл\n");return false;}
        else printf("Прочитано %lld байт из %lld (файл короче)\n", (*n), file_size);
    }
    buf[(*n)] = '\0';
    return true;
}

bool SetBuffer(long long file_size, FILE *fp) {
    if (file_size < MAX_BUFFER_SIZE) {

        char *mybuf = (char*)calloc(file_size, 1);
        if (!mybuf) {perror("calloc in SetBuffer");return false;}

        if (setvbuf(fp, mybuf, _IOFBF, file_size) != 0) {

            perror("setvbuf");
            free(mybuf);
            return false;
        }
        else {return true;}
    }
    else {printf("Слишком большой файл\n");return false;}
}

void CleanCRLF(long long *n, char *buf) {

    long long j = 0;
    for (long long i = 0; i < (*n); i++) {
        if (buf[i] != '\r') buf[j++] = buf[i];
    }

    (*n) = j;
    buf[*n] = '\0';
}

long long CountRows(const long long size, const char *buf) {

    long long k = 0, i = 0;
    for (; i < size && buf[i] != '\0'; i++) {
        if (buf[i] == '\n') k++;
    }

    if (i > 0 && buf[i-1] != '\n') k++;
    return k;
}

char **CreareIndexBox(const char *buf, const long long size, long long *k) {

    (*k) = CountRows(size, buf);
    char **index_box = (char **)calloc((*k), sizeof(char *));

    if (!index_box) {perror("calloc index_box"); return NULL;}
    if (AssignPointers(buf, size, index_box, k) != (*k)) {return NULL;}

    return index_box;
}

long long AssignPointers(const char *buf, const long long size, char **index_box, long long *k) {

    index_box[0] = (char *)&buf[0];
    long long j = 1;

    for (long long i = 0; i < size && j < (*k); i++) {
        if (buf[i] == '\n') {

            index_box[j] = (char *)&buf[i+1];
            j++;
        }
    }
    return (*k);
}

void PrintBuffer(char **index_box, const long long size_index_box) {

    for (long long i = 0;i < size_index_box; i++) {
        for (char *p = index_box[i]; *p != '\n' && *p != '\0'; p++) putchar(*p);

        putchar('\n');
    }
}

int CompareDownFileFromBegin(const void *str_a, const void *str_b) {

    const char *a = *(const char **)str_a;
    const char *b = *(const char **)str_b;

    int i = CompareStr(a, b, &BeginStartCompare, &CompareDownChar);
    return i;
}

int CompareDownFileFromEnd(const void *str_a, const void *str_b) {

    const char *a = *(const char **)str_a;
    const char *b = *(const char **)str_b;

    int i = CompareStr(a, b, &EndStartCompare, &CompareDownChar);
    return i;
}

int CompareStr(const char *a, const char *b, ComparisonStart(*WhereStartCompare)(size_t len_a, size_t len_b),
                                                                   int(*CompareFunc)(const char a, const char b)) {
    size_t len_a = FindStrLen(a);
    size_t len_b = FindStrLen(b);
    ComparisonStart u = (*WhereStartCompare)(len_a, len_b);

    while (1) {
        SkipToLetter(a, b, &u, len_a, len_b);

        bool a_done = (u.i_a < 0 || u.i_a >= (int)len_a);
        bool b_done = (u.i_b < 0 || u.i_b >= (int)len_b);

        if (a_done && b_done) return a_equivalent_b;
        if (a_done) return b_before_a;
        if (b_done) return a_before_b;

        int cmp = CompareFunc(a[u.i_a], b[u.i_b]);
        if (cmp > 0) return a_before_b;
        if (cmp < 0) return b_before_a;

        u.i_a += u.delta;
        u.i_b += u.delta;
    }
}

size_t FindStrLen(const char *row) {

    size_t i = 0;
    while (row[i] != '\n' && row[i] != '\0') {
        i++;
    }
    return i;
}

void SkipToLetter(const char *a, const char *b, ComparisonStart *u, size_t len_a, size_t len_b) {

    while (!isalpha(a[u->i_a]) && (u->i_a >= 0 && u->i_a < (int)len_a)) u->i_a += u->delta;
    while (!isalpha(b[u->i_b]) && (u->i_b >= 0 && u->i_b < (int)len_b)) u->i_b += u->delta;
}

int CompareDownChar(const char a, const char b) {

    return (a > b) - (a < b);      // не a-b, избегаем переполнения
}

ComparisonStart BeginStartCompare(size_t len_a, size_t len_b) {

    ComparisonStart u = {.i_a = 0, .i_b = 0, .delta = 1};
    return u;
}

ComparisonStart EndStartCompare(size_t len_a, size_t len_b) {

    ComparisonStart u = {.i_a = 0, .i_b = 0, .delta = -1};

    u.i_a = (len_a > 0) ? (int)len_a - 1 : 0;   // проверка для len 0
    u.i_b = (len_b > 0) ? (int)len_b - 1 : 0;
    return u;
}

int CreateOutputDataFile (const char *file_name) {
    int fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
    }
    return fd;
}

int WriteFileViaOpen(int fd, const char *buf, const long long size) {
    if (size < 0) {
        printf("Не удалось записать. -1 - неподходящий размер.\n");
        return 1;
    }
    size_t total_unwritten = (size_t)size;  //временные переменные записи
    char *p = (char*)buf;

    while (total_unwritten > 0) {
        ssize_t written = write(fd, (void *)p, total_unwritten);

        if (written == -1) {
            if (errno == EINTR) continue; // сигнал прервал, пробуем снова. пропускаем всё ниже и идём сразу к while
            perror("write");
            return 1;
        }
        p += written;  //сдвигаемся на непрочитанное
        total_unwritten -= (size_t)written;
    }
    return 0;
}

FILE *FCreateOutputDataFile (const char *file_name) {
    FILE *fp = fopen(file_name, "w");
    if (!fp) {
        perror("fopen");
        return NULL;
    }
    return fp;
}

int WriteFileViaFopen(FILE *fp, char **index_box, const long long size_index_box) {

    for (long long i = 0;i < size_index_box; i++) {

        size_t str_len = FindStrLen(index_box[i]);
        size_t written = fwrite(index_box[i], 1, str_len, fp);

        if (written != str_len) {
            perror("fwrite");
            return 1;
        }
        if (fputc('\n', fp) == EOF) {
            perror("fputc");
            return 1;
        }
    }
    const char *end = "----------------------------------------------------------------------------------------------------------";
    size_t written = fwrite(end, 1, strlen(end), fp);

    if (written != strlen(end)) {
        perror("fwrite");
        return 1;
    }
    if (fputc('\n', fp) == EOF) {
        perror("fputc");
        return 1;
    }
    fflush(fp);
    return 0;
}

int CompareDownPointers(const void *value_a, const void *value_b) {
    const int *a = *(const int**)value_a;
    const int *b = *(const int**)value_b;
    return (a > b) - (a < b);
}
//todo enum error
