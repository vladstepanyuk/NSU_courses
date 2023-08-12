//
#include "Matrix.h"

Matrix::Matrix(std::vector<float> matrix_in_vector, int line_size, int column_size): line_size(line_size), column_size(column_size) {
    matrix = std::vector<Vector>(column_size);
    for (int i = 0; i < column_size; ++i) {
        matrix[i] = Vector(line_size);
        for (int j = 0; j < line_size; ++j) {
            matrix[i][j] = matrix_in_vector[i * line_size + j];
        }
    }
}

void Matrix::matrix_vector_multiplication(Vector &vector, Vector &result) {
    for (int i = 0; i < column_size; ++i) {
        result[i] = matrix[i] * vector;
    }
}


Vector::Vector(const float *vector_in_vector, int N): N(N) {
    vector = (float *) _mm_malloc(N * sizeof(float), 16);
    for (int i = 0; i < N; ++i) {
        vector[i] = vector_in_vector[i];
    }
}

Vector::Vector(size_t N): N(N) {
    vector = (float *)_mm_malloc(N * sizeof(float), 16);
}

float &Vector::operator[](int i) {
    return vector[i];
}

Vector::~Vector() {
    if (vector != nullptr){
        _mm_free(vector);
    }
}

float Vector::operator*(Vector &vector1) {
    __m128 *xx, *yy;
    __m128 p, s;
    xx = (__m128 *) this->vector;
    yy = (__m128 *) vector1.vector;
    s = _mm_setzero_ps();
    for (int i = 0; i < N / 4; ++i) {
        p = _mm_mul_ps(xx[i], yy[i]);
        s = _mm_add_ps(s, p);
    }
    float tmp[4];
    _mm_store_ps(tmp, s);
    float sum = 0;
    for (float i : tmp) {
        sum += i;
    }
    for (int i = (N/4) * 4; i < N; ++i) {
        sum += vector[i] * vector1[i];
    }
    return sum;
}

void Vector::plus_vec(Vector &y, Vector &res) const {
    __m128 *xx, *yy;
    __m128 p;
    xx = (__m128 *) vector;
    yy = (__m128 *) y.vector;
    for (int i = 0; i < N / 4; ++i) {
        p = _mm_add_ps(xx[i], yy[i]);
        _mm_store_ps(res.vector + 4 * i, p);
    }

    for (int i = (N / 4) * 4; i < N; ++i) {
        res[i] = vector[i] + y[i];
    }
}

void Vector::minus_vec(Vector &y, Vector &res) const {
    __m128 *xx, *yy;
    __m128 p;
    xx = (__m128 *) vector;
    yy = (__m128 *) y.vector;
    for (int i = 0; i < N / 4; ++i) {
        p = _mm_sub_ps(xx[i], yy[i]);
        _mm_store_ps(res.vector + 4 * i, p);
    }

    for (int i = (N / 4) * 4; i < N; ++i) {
        res[i] = vector[i] - y[i];
    }
}

void Vector::vector_scalar_multiplication(Vector &result, float c) const {
    __m128 cc = _mm_set1_ps(c);
    __m128 res;
    __m128 *str = (__m128 *) vector;


    for (int i = 0; i < N / 4; ++i) {
        res = _mm_mul_ps(str[i], cc);
        _mm_store_ps(&result.vector[i * 4], res);
    }

    for (int i = (N / 4) * 4; i < N; ++i)
        result[i] = c * vector[i];
}

Vector::Vector(size_t N, float val): N(N) {
    vector = (float *)_mm_malloc(N * sizeof(float), 16);
    __m128 cc = _mm_set1_ps(val);

    for (int i = 0; i < N/4; ++i) {
        _mm_store_ps(&vector[i * 4], cc);
    }

    for (int i = (N/4) * 4; i < N; ++i) {
        vector[i] = val;
    }
}

size_t Vector::size() {
    return N;
}

Vector::Vector() {
    vector = nullptr;
    N = 0;
}

Vector::Vector(Vector &vector1): N(vector1.N){
    vector = (float *)_mm_malloc(vector1.N * sizeof(float), 16);
    __m128 *xx;

    xx = (__m128 *) vector1.vector;
    for (int i = 0; i < N/4; ++i) {
        _mm_store_ps(vector + i * 4, xx[i]);
    }

    for (int i = (N/4)*4; i < N; ++i) {
        vector[i] = vector1[i];
    }
}

Vector& Vector::operator=(Vector &vector1) {
    if (this == &vector1) return *this;
    if (vector != nullptr){
        _mm_free(vector);
    }
    vector = (float *)_mm_malloc(vector1.N * sizeof(float), 16);
    __m128 *xx;

    xx = (__m128 *) vector1.vector;
    for (int i = 0; i < N/4; ++i) {
        _mm_store_ps(vector + i * 4, xx[i]);
    }

    for (int i = (N/4)*4; i < N; ++i) {
        vector[i] = vector1[i];
    }
    return *this;
}

Vector& Vector::operator=(Vector &&vector1) {
    if (this == &vector1) return *this;
    if (vector != nullptr){
        _mm_free(vector);
    }
    vector = vector1.vector;
    N = vector1.N;
    vector1.vector = nullptr;
    vector1.N = 0;
    return *this;
}

