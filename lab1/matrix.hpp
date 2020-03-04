#pragma once
#include <cstddef>
#include <type_traits>
#include <vector>
#include <stdexcept>

template<typename T = int>
class Matrix {
    static_assert(std::is_arithmetic_v<T>, "Matrix should be numeric");

public:
    Matrix(size_t n, size_t m) : n_(n), m_(m), data_(n, std::vector<int>(m)) {
        //empty
    }

    Matrix(std::initializer_list<std::initializer_list<T>> list)
            : n_(list.size()), m_(list.begin()->size()), data_() {
        for (auto it = list.begin(); it != list.end(); ++it) {
            if (it->size() != m_) {
                throw std::runtime_error("Matrix is not rectangular");
            }
        }
        std::move(list.begin(), list.end(), std::back_inserter(data_));
    }

    Matrix() = default;

    Matrix(const Matrix<T>& other) = delete;

    Matrix(Matrix<T>&& other) = default;

    Matrix<T>& operator=(Matrix<T> const &other) = delete;

    Matrix<T>& operator=(Matrix<T> &&other) = default;

    ~Matrix() = default;

    decltype(auto) operator[](size_t idx) {
        return data_[idx];
    };

    decltype(auto) operator[](size_t idx) const {
        return data_[idx];
    };

    size_t width() const {
        return m_;
    }

    size_t height() const {
        return n_;
    }

    friend std::ostream& operator<<(std::ostream& output_stream, const Matrix& matrix) {
        output_stream << matrix.height() << " " << matrix.width() << std::endl;
        for (auto it = matrix.data_.begin(); it != matrix.data_.end(); ++it) {
            std::copy(it->begin(), it->end(), std::experimental::make_ostream_joiner(output_stream, " "));
            output_stream << std::endl;
        }
        return output_stream;
    }

private:
    size_t n_;
    size_t m_;
    std::vector<std::vector<T>> data_;
};