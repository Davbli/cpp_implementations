#include <iostream>

template <typename T>
class VVector {
private:
    T* begin_;
    T* end_;
    T* cap_;
public:
    Vector() : begin_(nullptr), end_(nullptr), cap_(nullptr) {}
    Vector(size_t n, const T& val) {
        void* raw = ::operator new(n * sizeof(T));
        begin_ = static_cast<T*>(raw);
        end_ = begin_;
        cap_ = begin_ + n;

        for (size_t i = 0; i < n; i++) {
            std::construct_at(end_, val);
            ++end_;
        }
    }
    ~Vector() {
        for (size_t i = 0; i < size(); i++) {
            begin_[i].~T();
        }
        ::operator delete(begin_);
    }

    Vector (Vector&& other) noexcept {
        ::operator delete(begin_);
        begin_ = other.begin_;
        end_ = other.end_;
        cap_ = other.cap_;
        other.begin_ = other.end_ = other.cap_ = nullptr;
    }

    Vector (const Vector& other) {
        size_t cap = other.capacity();
        size_t size = other.size();
        void* raw = ::operator new(cap * sizeof(T));
        begin_ = static_cast<T*>(raw);
        cap_ = begin_ + cap;
        end_ = begin_ + size;
        for (size_t i = 0; i < size; i++) {
            std::construct_at(begin_ + i, other[i]);
        }
    }

    Vector& operator=(const Vector& other) {
        if (this == &other) {
            return *this;
        }

        Vector tmp(other);
        std::swap(begin_, tmp.begin_);
        std::swap(end_, tmp.end_);
        std::swap(cap_, tmp.cap_);
        return *this;
    }

    Vector& operator=(Vector&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        for (size_t i = 0; i < size(); i++) {
            begin_[i].~T();
        }
        ::operator delete(begin_);
        begin_ = other.begin_;
        end_ = other.end_;
        cap_ = other.cap_;
        other.begin_ = other.end_ = other.cap_ = nullptr;
        return *this;
    }

    T& operator[](size_t i) {
        return begin_[i];
    }

    const T& operator[](size_t i) {
        return begin_[i];
    }
    void resize() {
        size_t oldSize = size();
        size_t newSize = (capacity() == 0) ? 1 : capacity() * 2;
        void* raw = ::operator new(newSize * sizeof(T));
        T* newBegin_ = static_cast<T*>(raw);
        end_ = newBegin_;
        cap_ = newBegin_ + newSize;
        for (size_t i = 0; i < oldSize; i++) {
            std::construct_at(newBegin_ + i, std::move(begin_[i]));
            end_++;
        }
        for (size_t i = 0; i < oldSize; i++) {
            begin_[i].~T();
        }
        ::operator delete(begin_);
        begin_ = newBegin_;
    }
    void push_back(const T& val) {
        if (end_ == cap_) {
            resize();
        }
        std::construct_at(end_, val);
        ++end;
    }

    void push_back(T&& val) {
        if (end_ == cap_) {
            resize();
        }
        std::construct_at(end_, std::move(val));
        ++end;
    }

    size_t capacity() {
        return cap_ - begin_;
    }

    size_t size() {
        return end_ - begin_;
    }
    bool empty() {
        return begin_ == end_;
    }
};

int main() {
    return 0;
}
