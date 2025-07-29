#pragma once
#include <cassert>
#include <cmath>
#include <iostream>
#include <type_traits>

template <class T, int n>
    requires(std::is_arithmetic_v<T>)
struct vec {
    T  data[n] = {0};
    T& operator[](const int i) {
        assert(i >= 0 && i < n);
        return data[i];
    }
    T operator[](const int i) const {
        assert(i >= 0 && i < n);
        return data[i];
    }
};

template <class T, int n>
    requires(std::is_arithmetic_v<T>)
T operator*(const vec<T, n>& lhs, const vec<T, n>& rhs) {
    T ret = 0; // N.B. Do not ever, ever use such for loops! They are highly confusing.
    for(int i = n; i--; ret += lhs[i] * rhs[i])
        ;       // Here I used them as a tribute to old-school game programmers fighting for every CPU cycle.
    return ret; // Once upon a time reverse loops were faster than the normal ones, it is not the case anymore.
}

template <class T, int n>
    requires(std::is_arithmetic_v<T>)
vec<T, n> operator+(const vec<T, n>& lhs, const vec<T, n>& rhs) {
    vec<T, n> ret = lhs;
    for(int i = n; i--; ret[i] += rhs[i])
        ;
    return ret;
}

template <class T, int n>
    requires(std::is_arithmetic_v<T>)
vec<T, n> operator-(const vec<T, n>& lhs, const vec<T, n>& rhs) {
    vec<T, n> ret = lhs;
    for(int i = n; i--; ret[i] -= rhs[i])
        ;
    return ret;
}

template <class T, int n>
    requires(std::is_arithmetic_v<T>)
vec<T, n> operator*(const vec<T, n>& lhs, const double& rhs) {
    vec<T, n> ret = lhs;
    for(int i = n; i--; ret[i] *= rhs)
        ;
    return ret;
}

template <class T, int n>
    requires(std::is_arithmetic_v<T>)
vec<T, n> operator*(const double& lhs, const vec<T, n>& rhs) {
    return rhs * lhs;
}

template <class T, int n>
    requires(std::is_arithmetic_v<T>)
vec<T, n> operator/(const vec<T, n>& lhs, const double& rhs) {
    vec<T, n> ret = lhs;
    for(int i = n; i--; ret[i] /= rhs)
        ;
    return ret;
}

template <class T, int n>
    requires(std::is_arithmetic_v<T>)
std::ostream& operator<<(std::ostream& out, const vec<T, n>& v) {
    for(int i = 0; i < n; i++)
        out << v[i] << " ";
    return out;
}

template <class T>
    requires(std::is_arithmetic_v<T>)
struct vec<T, 2> {
    T  x = 0, y = 0;
    T& operator[](const int i) {
        assert(i >= 0 && i < 2);
        return i ? y : x;
    }
    T operator[](const int i) const {
        assert(i >= 0 && i < 2);
        return i ? y : x;
    }
};

template <class T>
    requires(std::is_arithmetic_v<T>)
struct vec<T, 3> {
    T  x = 0, y = 0, z = 0;
    T& operator[](const int i) {
        assert(i >= 0 && i < 3);
        return i ? (1 == i ? y : z) : x;
    }
    T operator[](const int i) const {
        assert(i >= 0 && i < 3);
        return i ? (1 == i ? y : z) : x;
    }
};

template <class T>
    requires(std::is_arithmetic_v<T>)
struct vec<T, 4> {
    T  x = 0, y = 0, z = 0, w = 0;
    T& operator[](const int i) {
        assert(i >= 0 && i < 4);
        return i < 2 ? (i ? y : x) : (2 == i ? z : w);
    }
    T operator[](const int i) const {
        assert(i >= 0 && i < 4);
        return i < 2 ? (i ? y : x) : (2 == i ? z : w);
    }
    vec<T, 2> xy() const { return {x, y}; }
    vec<T, 3> xyz() const { return {x, y, z}; }
};

template <class T>
    requires(std::is_arithmetic_v<T>)
using vec2  = vec<T, 2>;
using Vec2i = vec2<int32_t>;
using Vec2l = vec2<int64_t>;
using Vec2f = vec2<float>;
using Vec2d = vec2<double>;
template <class T>
    requires(std::is_arithmetic_v<T>)
using vec3  = vec<T, 3>;
using Vec3i = vec3<int32_t>;
using Vec3l = vec3<int64_t>;
using Vec3f = vec3<float>;
using Vec3d = vec3<double>;
template <class T>
    requires(std::is_arithmetic_v<T>)
using vec4  = vec<T, 4>;
using Vec4i = vec4<int32_t>;
using Vec4l = vec4<int64_t>;
using Vec4f = vec4<float>;
using Vec4d = vec4<double>;

template <class T, int n>
    requires(std::is_arithmetic_v<T>)
double norm(const vec<T, n>& v) {
    return std::sqrt(v * v);
}

template <class T, int n>
    requires(std::is_arithmetic_v<T>)
vec<T, n> normalized(const vec<T, n>& v) {
    return v / norm(v);
}

template <class T>
    requires(std::is_arithmetic_v<T>)
inline vec3<T> cross(const vec3<T>& v1, const vec3<T>& v2) {
    return {v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x};
}

template <int n>
struct dt;

template <int nrows, int ncols>
struct mat {
    vec<double, ncols> rows[nrows] = {{}};

    vec<double, ncols>& operator[](const int idx) {
        assert(idx >= 0 && idx < nrows);
        return rows[idx];
    }
    const vec<double, ncols>& operator[](const int idx) const {
        assert(idx >= 0 && idx < nrows);
        return rows[idx];
    }

    double det() const {
        return dt<ncols>::det(*this);
    }

    double cofactor(const int row, const int col) const {
        mat<nrows - 1, ncols - 1> submatrix;
        for(int i = nrows - 1; i--;)
            for(int j = ncols - 1; j--; submatrix[i][j] = rows[i + int(i >= row)][j + int(j >= col)])
                ;
        return submatrix.det() * ((row + col) % 2 ? -1 : 1);
    }

    mat<nrows, ncols> invert_transpose() const {
        mat<nrows, ncols> adjugate_transpose; // transpose to ease determinant computation, check the last line
        for(int i = nrows; i--;)
            for(int j = ncols; j--; adjugate_transpose[i][j] = cofactor(i, j))
                ;
        return adjugate_transpose / (adjugate_transpose[0] * rows[0]);
    }

    mat<nrows, ncols> invert() const {
        return invert_transpose().transpose();
    }

    mat<ncols, nrows> transpose() const {
        mat<ncols, nrows> ret;
        for(int i = ncols; i--;)
            for(int j = nrows; j--; ret[i][j] = rows[j][i])
                ;
        return ret;
    }
};

template <class T, int nrows, int ncols>
    requires(std::is_arithmetic_v<T>)
vec<T, ncols> operator*(const vec<T, nrows>& lhs, const mat<nrows, ncols>& rhs) {
    return (mat<1, nrows>{{lhs}} * rhs)[0];
}

template <class T, int nrows, int ncols>
    requires(std::is_arithmetic_v<T>)
vec<T, nrows> operator*(const mat<nrows, ncols>& lhs, const vec<T, ncols>& rhs) {
    vec<T, nrows> ret;
    for(int i = nrows; i--; ret[i] = lhs[i] * rhs)
        ;
    return ret;
}

template <int R1, int C1, int C2>
mat<R1, C2> operator*(const mat<R1, C1>& lhs, const mat<C1, C2>& rhs) {
    mat<R1, C2> result;
    for(int i = R1; i--;)
        for(int j = C2; j--;)
            for(int k = C1; k--; result[i][j] += lhs[i][k] * rhs[k][j])
                ;
    return result;
}

template <int nrows, int ncols>
mat<nrows, ncols> operator*(const mat<nrows, ncols>& lhs, const double& val) {
    mat<nrows, ncols> result;
    for(int i = nrows; i--; result[i] = lhs[i] * val)
        ;
    return result;
}

template <int nrows, int ncols>
mat<nrows, ncols> operator/(const mat<nrows, ncols>& lhs, const double& val) {
    mat<nrows, ncols> result;
    for(int i = nrows; i--; result[i] = lhs[i] / val)
        ;
    return result;
}

template <int nrows, int ncols>
mat<nrows, ncols> operator+(const mat<nrows, ncols>& lhs, const mat<nrows, ncols>& rhs) {
    mat<nrows, ncols> result;
    for(int i = nrows; i--;)
        for(int j = ncols; j--; result[i][j] = lhs[i][j] + rhs[i][j])
            ;
    return result;
}

template <int nrows, int ncols>
mat<nrows, ncols> operator-(const mat<nrows, ncols>& lhs, const mat<nrows, ncols>& rhs) {
    mat<nrows, ncols> result;
    for(int i = nrows; i--;)
        for(int j = ncols; j--; result[i][j] = lhs[i][j] - rhs[i][j])
            ;
    return result;
}

template <int nrows, int ncols>
std::ostream& operator<<(std::ostream& out, const mat<nrows, ncols>& m) {
    for(int i = 0; i < nrows; i++)
        out << m[i] << std::endl;
    return out;
}

template <int n>
struct dt { // template metaprogramming to compute the determinant recursively
    static double det(const mat<n, n>& src) {
        double ret = 0;
        for(int i = n; i--; ret += src[0][i] * src.cofactor(0, i))
            ;
        return ret;
    }
};

template <>
struct dt<1> { // template specialization to stop the recursion
    static double det(const mat<1, 1>& src) {
        return src[0][0];
    }
};
