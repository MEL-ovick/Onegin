#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Rand.c"
#include "gettime.h"

typedef struct {
    int *left, *right, *pivot_box;
    size_t left_size, right_size, pivot_size;
} PartitionResult;

typedef struct {
    const char *name;
    int(*CompareFunc)(const void *value_a, const void *value_b);
    void (*SortFunc)(void *data, size_t size, size_t type, int(*CompareFunc)(const void *value_a, const void *value_b));
    void *data;
    size_t size, type;
} SortFuncParametrs;

void bSort                               (int *data, size_t size);
void ChangeValuesInt              (int *a, int *b);
void ChangeValues                   (void *a, void *b, size_t type);
void PrintArray                       (int *data, size_t size);
void qSort1                             (int *data, size_t size, int (*CompareFunc)(int a, int b));
void TestIntSort                    (int *data, size_t size);
void TestQsort                       (int *data, size_t size);
int   CompareUp                      (const void *value_a, const void *value_b);
int   CompareUpInt                 (int a, int b);
PartitionResult PartitionArray  (int *data, size_t size, size_t pivot, int(*CompareFunc)(int a, int b));
void MergeBack                      (int *data, const PartitionResult *res);
void qSort                              (void *data, size_t size, size_t type, int(*CompareFunc)(const void *value_a, const void *value_b));
void TestQsort1                     (int *data, size_t size);
void insSort                            (int *data, size_t size);
double TimeTest                     (const SortFuncParametrs *t);
/*
int main() {
    int data[] = {50, 53, 60, 54, 49,  70, 48};
    size_t size = sizeof(data) / sizeof(data[0]);
    TestQsort(data, size);
    return 0;
}
*/
void bSort(int *data, size_t size) {
    if (size <= 1 || data == NULL) return;
    for (int nPass = 0; nPass < (size - 1); nPass++) {
        for (int i = 0, sizek = size-nPass-1; i < sizek; i++) {
            if (data[i] > data[i+1]) {
                ChangeValuesInt(&data[i], &data[i+1]);
            }
        }
    }
}

void insSort(int *data, size_t size) {
    if (size <= 1 || data == NULL) return;
    for (int i = 0; i < (size-1); i++) {
        for (int j = i; j >= 0; j--) {
            if (data[j] > data [j+1]) {
                ChangeValuesInt(&data[j], &data[j+1]);
            }
            else break;
        }
    }
}

void selecSort(int *data, size_t size) {
    if (size <= 1 || data == NULL) return;
    for (int nPass = 0; nPass < size; nPass++) {
        int min = nPass, i = nPass;
        for (; i < size; i++) {
            if (data[i] < data[min]) {
                min = i;
            }
        }
        ChangeValuesInt(&data[nPass], &data[min]);
    }
}

void mergeSort(int *data, size_t size) {                //недоделал
    if (size <= 1 || data == NULL) return;
    //разбиваем на коробочки размером 1 и группируем обратно сортировкой парных инвариантов
    size_t similar_boxes = size /2, dissimilar_boxes = size % 2, size_similar = 2, size_dissimilar = dissimilar_boxes;
    while (simmilar_boxes > 0) {
        for (int i = 0; i < similar_boxes; i++) {
            if (!SortPair(data[i*size_similar], size_similar)) printf("error\n");   //сортируем линейный инвариант
        }
    }
}
                                      //это доделал
bool SortPair(int *data, size_t size) {     //сортируем линейный инвариант
    if (size % 2 != 0 || data == NULL) return false;
    size_t compare1 = 0, compare2=size/2, value1 = 0, value2 = size/2, balance = 0, solutions = 0, depth = 0;
    while(solutions < (size/2)*(size/2)) {
        if (value1 < size/2) {
            if (data[compare1] > data[compare2]) {
                value2 = size/2 + balance;
                ChangeValuesInt(data[value1], data[value2]);
                solutions += ((size/2) - (depth - balance));
                balance++;
                compare1 = size/2;
                compare2 = size/2 + balance;
            }
            else if (balance >= 1) {
                value2 = size/2;
                ChangeValuesInt(data[value1], data[value2]);
                for (int i = 0; i < balance; i++) {
                    ChangeValues(data[value2+i], data[value2+i+1]);
                }
                solutions += ((size/2) - balance);
                compare1 = size/2;
                compare2 = size/2 + balance;
            }
            else {
                solutions += ((size/2) - balance);
                compare1++;
            }
            value1++;
            depth++;
        }
        SortDissimilarPair(&data[size/2], size/2, balance);      //сортируем нелинейный остаток
    }
    return true;
}
                                   //а это нет
void SortDissimilarPair(int *data, size_t size, size_t left_size) {
    if (size <= 1 || data == NULL || left_size == 0 || left_size == size) return;
}

void ChangeValuesInt(int *a, int *b) {
    assert(a != NULL);
    assert(b != NULL);

    int temp = *b;
    *b = *a;
    *a = temp;
}

void ChangeValues(void *a, void *b, size_t type) {
    assert(a != NULL);
    assert(b != NULL);

    unsigned char t = '\0';
    for (size_t i = 0; i < type; i++) {
        t = *((unsigned char*)b + i);
        *((unsigned char*)b + i) = *((unsigned char*)a + i);
        *((unsigned char*)a + i) = t;
    }
}

void PrintArray(int *data, size_t size) {
    assert(data != NULL);

    int i = 0;
    for (; i < size; i++) {
        printf("%d ", data[i]);
    }
    printf("\n");
}

//шуточная версия сортировки, напоминающая qsort с гугл картинок
//рекурсивный вызов с динамическими массивами.
void qSort1(int *data, size_t size, int (*CompareFunc)(int a, int b)) {
    if (size <= 1 || data == NULL) {return;}

    size_t pivot = Rand(0, size-1);
    PartitionResult part = PartitionArray(data, size, pivot, CompareFunc);

    if (part.left != NULL) {
        qSort1(part.left, part.left_size, CompareFunc);
    }
    if (part.right != NULL) {
        qSort1(part.right, part.right_size, CompareFunc);
    }

    MergeBack(data, &part);
    free(part.left);
    free(part.pivot_box);
    free(part.right);
}

PartitionResult PartitionArray(int *data, size_t size, size_t pivot, int(*CompareFunc)(int a, int b)) {
    PartitionResult res = {NULL, NULL, NULL, 0, 0, 0};
    for (int i = 0; i < size; i++) {
        int cmp = (*CompareFunc)(data[i], data[pivot]);
        if (cmp == 0) {
            res.pivot_size++;
            int *tmp = (int *)realloc(res.pivot_box, res.pivot_size*sizeof(int));
            if (!tmp) {printf("Ошибка выделения памяти для pivot\n"); exit(1);}
            res.pivot_box = tmp;
            res.pivot_box[res.pivot_size-1] = data[i];
        }
        else if (cmp < 0) {
            res.left_size++;
            int *tmp = (int *)realloc(res.left, res.left_size*sizeof(int));
            if (!tmp) {printf("Ошибка выделения памяти для left\n"); exit(1);}
            res.left = tmp;
            res.left[res.left_size-1] = data[i];
        }
        else if (cmp > 0) {
            res.right_size++;
            int *tmp = (int *)realloc(res.right, res.right_size*sizeof(int));
            if (!tmp) {printf("Ошибка выделения памяти для right\n"); exit(1);}
            res.right = tmp;
            res.right[res.right_size-1] = data[i];
        }
        else {printf("Не удалось сравнить\n");exit(1);}
    }
    return res;
}

void MergeBack(int *data, const PartitionResult *res) {
    int l = 0, p = 0, r = 0, done = 0;
    size_t size = res->left_size + res->pivot_size + res->right_size;
    for (int i = 0; i < size; i++) {
        done = 0;
        if (l < res->left_size && done == 0) {
            data[i] = res->left[l];
            l++;
            done = 1;
        }
        if (p < res->pivot_size && done == 0) {
            data[i] = res->pivot_box[p];
            p++;
            done = 1;
        }
        if (r < res->right_size && done == 0) {
            data[i] = res->right[r];
            r++;
            done = 1;
        }
    }
}

void TestQsort1(int *data, size_t size) {
    assert(data != NULL);
    qSort1(data, size, &CompareUpInt);
    PrintArray(data, size);
}

void qSort(void *data, size_t size, size_t type, int(*CompareFunc)(const void *value_a, const void *value_b)) {
    if (size <= 1 || data == NULL) {return;}
    size_t pivot = Rand(0, size-1);
    size_t left = 0, right = size - 2;
    void *p = (unsigned char*)data + pivot * type;
    void *end = (unsigned char*)data + (size - 1) * type;
    ChangeValues(p, end, type);
    pivot = size - 1;
    p = (unsigned char*)data + pivot * type;
    void *l = (unsigned char*)data + left * type;
    void *r = (unsigned char*)data + right * type;
    while (left < right) {
        while (((*CompareFunc)(l, p) < 0) && left < right) {
            left++;
            l = (unsigned char*)data + left * type;
        }
        while (((*CompareFunc)(p, r) < 0) && left < right) {
            right--;
            r = (unsigned char*)data + right * type;
        }
        if (left < right) {
            ChangeValues(l, r, type);
            left++;
            right--;
            l = (unsigned char*)data + left * type;
            r = (unsigned char*)data + right * type;
        }
    }
    if ((*CompareFunc)(l, p) < 0) {
        left++;
        l = (unsigned char*)data + left * type;
    }
    ChangeValues(l, p, type);
    qSort(data, left, type, CompareFunc);
    qSort((unsigned char*)data + (left+1) * type, size - left-1, type, CompareFunc);
}

void TestIntSort(int *data, size_t size) {
    assert(data != NULL);
    selecSort(data, size);
    PrintArray(data, size);
}

void TestQsort(int *data, size_t size) {
    assert(data != NULL);

    int *buf = (int *)calloc(size, sizeof(data[0]));
    memcpy(buf, data, size*sizeof(data[0]));

    SortFuncParametrs test_qSort = {
        .name = "qSort",
        .CompareFunc = &CompareUp, .SortFunc= qSort,
        .data = (void *)buf, .size = size, .type = sizeof(data[0])
    };
    TimeTest(&test_qSort);

    memcpy(buf, data, size*sizeof(data[0]));

    SortFuncParametrs test_qsort = {
        .name = "qsort",
        .CompareFunc = &CompareUp, .SortFunc= qsort,
        .data = (void *)buf, .size = size, .type = sizeof(data[0])
    };
    TimeTest(&test_qsort);

    //qSort((void *)data, size, sizeof(data[0]), &CompareUp);
    //PrintArray(data, size);

    free(buf);
}

double TimeTest(const SortFuncParametrs *t) {

    double start_time = get_time_sec();
    t->SortFunc(t->data, t->size, t->type, t->CompareFunc);
    double end_time = get_time_sec();

    PrintArray((int*)(t->data), t->size);
    printf("%-20s : %.9f сек\n", t->name, end_time - start_time);
    return end_time - start_time;
}

int CompareUp(const void *value_a, const void *value_b) {
    const int a = *(const int*)value_a;
    const int b = *(const int*)value_b;
    return (a > b) - (a < b);
}

int CompareUpInt(int a, int b) {
    return (a > b) - (a < b);      // не a-b, избегаем переполнения
}
