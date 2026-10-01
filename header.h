// header.h для всей системы файлов решения квадратного уравнения
#ifndef HEADER_H
#define HEADER_H

// библиотеки
#include <stdio.h>
#include <windows.h>
#include <complex.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <math.h>
#include <stdarg.h>

// макросы
#define ENCODING 1251
#define ARRAY_DEFAULT_SIZE 50
#define A_BOX_ARRAY_DEFAULT_SIZE 8

#define COEFFS_NOT_CORRECT    (!(is_zero (a - test.a_ref) && is_zero(b - test.b_ref) && is_zero(c - test.c_ref)))
#define IS_NUMBER(c)                   (isdigit(c) || c == '.' || c == '-')
#define NUMBER_END(c)                (c == '*' || c == ' ' || c == 'x' || c == '\0' || c == '+')
#define X_OR_END(c)                     (c == 'x' || c == '\0' || c == '+' || c == '-')
#define DEGREE_OR_END(c)           (c == '^' || c == '\0' || c == '+' || c == '*' || c == '-')
#define NUM_OR_X(c)                    (isdigit(c) || c == '.' || c == '\0' || c == '-' || c == 'x')
#define NEXT_SYM_END(c)            (c == ' ' || c == '+' || c == '\0')

#define CLR "\033[%dm"
#define CLREND "\033[0m"
#define CLRVIOLET "\033[35m"

#define CLRSUCCESS(message) "\033[32m" message CLREND
#define CLRERROR(message) "\033[31m" message CLREND
#define CLRHIGHLIGHT(message) "\033[36m" message CLREND
#define CLRBLUE(message) "\033[34m" message CLREND
#define CLRCURSOR(message)       "\033[33m" message CLREND

#define ASSERT(condition) \
    if (!(condition)) { \
        printf("Ошибка тут" #condition);\
        abort();\
    }

#define BLACK 30
#define RED 31
#define GREEN 32
#define YELLOW 33
#define BLUE 34
#define VIOLET 35
#define TURQUOISE 36
#define WHITE 37

// константы
const int     x_by_default = 997;
const float  calc_error = (float)1.0e-8;
const int     defaltr = -20;
const int     scanned_succesfully = 1;
const int     hundred = 100;
const int     COEFF_1 = 1;
const char  degree_one = '1';
const char  degree_two = '2';
const int     num_cells_in_row = 4;

// enum
enum solutions {no_roots, one_root, two_roots, infinity, complex_roots};
enum monomial_degree {degree_0, degree_1, degree_2, not_square};
enum cells_in_row {CELL_1, CELL_2, CELL_3, CELL_4};

// структуры
struct TestParsing {
    char polinom [ARRAY_DEFAULT_SIZE];
    float a_ref, b_ref, c_ref;
};

struct TableQuantities {
    unsigned long count, num_cells, num_rows;
};

struct TableCounters {
    int row, cell, poli, a, b, c;
};

struct AdapterBoxes {
    char poli [ARRAY_DEFAULT_SIZE];
    char a [A_BOX_ARRAY_DEFAULT_SIZE];
    char b [A_BOX_ARRAY_DEFAULT_SIZE];
    char c [A_BOX_ARRAY_DEFAULT_SIZE];
};

// прототипы parsing
void parse                              (char *equat, int len, float *a, float *b, float *c);
int polinomials                         (char *equat, int len);
float parse_monomial             (char *equat, int len, int *k, int *degree);
int get_coeffs_to_parse         (float *a, float *b, float *c);
float run_all_tests                 (void);
bool run_one_test                  (struct TestParsing test);
bool is_zero                           (float s);
int print_cursor                      (char *equat, int k);
float find_number                  (char *equat, int *k);
int skip_from_coeff_to_x      (char *equat, int *k);
int skip_from_degree_to_exp (char *equat, int *k);
int skip_from_x_to_second_x (char *equat, int *k);
int skip_from_x_to_degree     (char *equat, int *k);
bool check_first_degree         (char *equat, int *k, int *degree);
bool check_null_coeff            (char *equat, int *k, int *degree);
int skip_to_num_or_x             (char *equat, int *k);
float write_a_number             (char *equat, int *k);
bool skip_minus                         (char *equat, int *k);
bool check_not_square           (char *equat, int *k, int *degree);
bool check_multiple_after_x  (char *equat, int *k, int *degree);
bool check_second_x             (char *equat, int *k, int *degree);
bool check_exp_is_one           (char *equat, int *k, int *degree);
bool check_exp_is_two           (char *equat, int *k, int *degree);
void change_last_sim              (char *equat, int len);
void degree_distributor           (int degree, float *a, float *b, float *c, float coeff);
bool find_degree                    (char *equat, int *k, int *degree);
bool processing_coeff            (char *equat, int *k, int *degree, float*a);
bool find_out_coeff_context (char *equat, int *k, int *degree);
bool find_exp                        (char *equat, int *k, int *degree);
float find_out_coeff             (char *equat, int *k, float *a);
void print_result_coeffs        (float a, float b, float c);
void processing_last_a_box_symbol(char *a_box, int a_finder);
void place_carriage_k            (char *equat, int k);
void place_second_border      (char *equat, int k);
float run_all_tests_via_file    (struct TestParsing *test, unsigned long *num_rows);
float count_procentage          (float size_testsf, int success);

// прототипы Kvadr_Urav
bool get_coeffs     (float *a, float *b, float *c);                                   // функция ввода
void lin_equat         (float b, float c, float x1);                                      // функция линейного уравнения
void two_sol           (float a, float b, float c, float x1, float x2);            // функция решения при положительном D
void one_sol           (float a, float b, float x1);                                      // функция решения при нулевом D
void complex_sol    (float a, float b, float c);                                        // функция решения при отрицательном D
int square_solver    (float a, float b, float c, float *x1, float *x2);         // распределительная функция решения
float linear_solver  (float a, float b);                                                   // решение линейного уравнения
bool is_zero           (float s);                                                               // сравнение с проверкой ошибок вычисления около 0
bool get_one_coeff(float *a, char coeff);                                            // функция ввода конкретного коефицента
void print_answer     (int n_roots, float x1, float x2);                            // функция распределения и печати ответов
int discriminant_distributor(float a,float b, float c, float *x1, float *x2);// функция решения с дискриминантом

// прототипы ParserTestReader
FILE * error_check              (int argc, char *argv[]);
void counting_table_cells       (int argc, char *argv[], struct TableQuantities *quan);
void processing_count           (FILE *fp, struct TableQuantities *quan);
void output_count                 (char *argv[], struct TableQuantities *quan);
void check_table                   (unsigned long num_cells);
int processing_test_text       (int argc, char *argv[], unsigned long *num_rows);
void processing_symbols        (FILE * fp, struct TableCounters *k, struct AdapterBoxes *box, struct TestParsing *test);
void processing_usefull_cells (int ch, struct TableCounters *k, struct AdapterBoxes *box, struct TestParsing *test);
int check_skip_headlines     (int ch);
void cell_distributor              (int ch, struct TableCounters *k, struct AdapterBoxes *box);
bool skip_headlines               (int *i);
void check_empty_space       (unsigned long *num_rows);
void rewrite_poli                   (char *poli_box, char *polinom, int size_box);
void rewrite_coeff               (char *coeff_box, float *coeff_ref);
int replace_0_sim                 (char *a_box, int size_box);
FILE * open_error               (const char* file_name);

#endif
