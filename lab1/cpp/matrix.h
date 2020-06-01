#pragma once
#include <cstddef>
#include <type_traits>
#include <vector>
#include <stdexcept>
#include <experimental/iterator>

template<typename T = int>
class Matrix {
    static_assert(std::is_arithmetic_v<T>, "Matrix should be numeric");

public:
    Matrix(size_t n, size_t m) : n_{n}, m_{m}, data_(n*m) {
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

    Matrix<T>& operator=(Matrix<T> const &other) = delete;

    Matrix<T>& operator=(Matrix<T> &&other) noexcept = default;

    ~Matrix() = default;

    decltype(auto) at(size_t row, size_t column) {
        return data_[row * m_ + column];
    }

    decltype(auto) at(size_t row, size_t column) const {
        return data_[row * m_ + column];
    }

    size_t width() const {
        return m_;
    }

    size_t height() const {
        return n_;
    }

    friend std::ostream& operator<<(std::ostream& output_stream, const Matrix& matrix) {
        output_stream << matrix.height() << " " << matrix.width() << std::endl;
        auto begin = matrix.data_.begin();
        for (auto i = 0; i < matrix.n_; ++i) {
            std::copy(begin + i * matrix.m_, begin + (i + 1) * matrix.m_,
                    std::experimental::make_ostream_joiner(output_stream, " "));
            output_stream << std::endl;
        }
        return output_stream;
    }

private:
    size_t n_;
    size_t m_;
    std::vector<T> data_;
};