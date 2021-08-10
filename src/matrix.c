#include "matrix.h"

struct matrix *
initmatrix(size_t rows, size_t cols)
{
    struct matrix *matrix = malloc(sizeof(struct matrix));
    if (matrix == NULL) {
        goto struct_alloc_err;
    }
    matrix->rows = 0;
    matrix->cols = cols;

    matrix->items = calloc(rows, sizeof(size_t *));
    if (matrix->items == NULL) {
        goto rows_alloc_err;
    }

    for (size_t row = 0; row < rows; ++row) {
        matrix->items[row] = calloc(cols, sizeof(size_t));
        if (matrix->items[row] == NULL) {
            goto cols_alloc_err;
        }

        ++matrix->rows;
    }
    assert(matrix->rows == rows);

    return matrix;

cols_alloc_err:
    for (size_t row = 0; row < matrix->rows; ++row) {
        free(matrix->items[row]);
    }
    free(matrix->items);
rows_alloc_err:
    free(matrix);
struct_alloc_err:
    return NULL;
}

void
freematrix(struct matrix *matrix)
{
    for (size_t row = 0; row < matrix->rows; ++row) {
        free(matrix->items[row]);
    }
    free(matrix->items);
    free(matrix);
}
