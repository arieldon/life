#ifndef MATRIX_H
#define MATRIX_H

#include <assert.h>
#include <stdlib.h>

struct matrix {
    size_t rows;
    size_t cols;
    size_t **items;
};

struct matrix *initmatrix(size_t rows, size_t cols);
void freematrix(struct matrix *matrix);

#endif
