#pragma once
#include <cstddef>
#include <type_traits>
#include <vector>
#include <stdexcept>
#include <experimental/iterator>

template<typename T = long>
class Matrix {
    static_assert(std::is_arithmetic_v<T>, "Matrix should be numeric");

public:
    Matrix(size_t n, size_t m) : n_{n}, m_{m}, data_(n*m) {
        //empty
    }

    Matrix(size_t n, size_t m, bool is_orc) : n_{n}, m_{m}, data_(n * m), is_orc(is_orc) {
        //empty
    }

    Matrix(std::initializer_list<std::initializer_list<T>> list)
            : n_(list.size()), m_(list.begin()->size()), data_() {
        for (auto it_row = list.begin(); it_row != list.end(); ++it_row) {
            if (it_row->size() != m_) {
                throw std::runtime_error("Matrix is not rectangular");
            }
            std::move(it_row->begin(), it_row->end(), std::back_inserter(data_));
        }
    }

    Matrix() = delete;

    Matrix(const Matrix<T>& other) = delete;

    Matrix(Matrix<T>&& other) noexcept = default;

    Matrix<T>& operator=(const Matrix<T> &other) = delete;

    Matrix<T>& operator=(Matrix<T> &&other) noexcept = default;

    ~Matrix() = default;

    inline decltype(auto) at(size_t row, size_t column) {
        return data_[is_orc ? (column * n_ + row) : (row * m_ + column)];
    }

    inline decltype(auto) at(size_t row, size_t column) const {
        return data_[is_orc ? (column * n_ + row) : (row * m_ + column)];
    }

    size_t width() const {
        return m_;
    }

    size_t height() const {
        return n_;
    }

private:
    size_t n_;
    size_t m_;
    std::vector<T> data_;
    bool is_orc = false;
};

template <typename T>
std::ostream& operator<<(std::ostream& output_stream, const Matrix<T>& matrix) {
    output_stream << matrix.height() << " " << matrix.width() << std::endl;
    for (size_t i = 0; i < matrix.height(); ++i) {
        for (size_t j = 0; j < matrix.width(); ++j) {
            output_stream << matrix.at(i, j);
        }
        output_stream << std::endl;
    }
    return output_stream;
}
