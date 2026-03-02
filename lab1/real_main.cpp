#include <iostream>
#include <stdexcept>
#include <string>
#include <cstddef>
#include <cstdint>


template <typename T>
class Vector {
    private:
        T* data;
        size_t size;
        size_t capacity;

        void reallocate(size_t new_capacity) {
            T* new_data = new T[new_capacity];
            for (size_t i = 0; i < this->size; i++) {
                new_data[i] = std::move(this->data[i]);
            }
            delete[] this->data;
            this->data = new_data;
            this->capacity = new_capacity;
        }
    
    public:
        Vector() : data(nullptr), size(0), capacity(0) {}

        Vector(size_t size, T value = T{}) : data(nullptr), size(size), capacity(size) {
             if (size > 0) {
                this->data = new T[size];
                for (size_t i = 0; i < size; i++) {
                    this->data[i] = value;
                }
            }
        }

        Vector(const Vector& other)
            : data(nullptr), size(other.size), capacity(other.capacity)
        {
            if (capacity > 0) {
                data = new T[capacity];
                for (size_t i = 0; i < size; ++i) {
                    data[i] = other.data[i];
                }
            }
        }

        Vector& operator=(const Vector& other)
        {
            if (this == &other)
                return *this;

            T* new_data = nullptr;

            if (other.capacity > 0) {
                new_data = new T[other.capacity];
                for (size_t i = 0; i < other.size; ++i) {
                    new_data[i] = other.data[i];
                }
            }

            delete[] data;

            data = new_data;
            size = other.size;
            capacity = other.capacity;

            return *this;
        }

        Vector(Vector&& other) noexcept
            : data(other.data),
            size(other.size),
            capacity(other.capacity)
        {
            other.data = nullptr;
            other.size = 0;
            other.capacity = 0;
        }

        Vector& operator=(Vector&& other) noexcept
        {
            if (this == &other)
                return *this;

            delete[] data;

            data = other.data;
            size = other.size;
            capacity = other.capacity;

            other.data = nullptr;
            other.size = 0;
            other.capacity = 0;

            return *this;
        }

        ~Vector() noexcept {
            delete[] data;
        }

        size_t Size() const {
            return this->size;
        }

        void PushBack(T&& value) {
            if (this->size == this->capacity) {
                size_t new_capacity;
                if (this->capacity == 0) {
                    new_capacity = 1;
                } else {
                    new_capacity = this->capacity * 2;
                }
                reallocate(new_capacity);
            }
            this->data[this->size] = std::move(value);
            this->size++;
        }

        friend std::ostream& operator<<(std::ostream& os, const Vector<T>& vector) {
            for (size_t i = 0; i < vector.size; i++) {
                os << vector.data[i] << "\n";
            }
            return os;
        }

        T& operator[](size_t idx) {
            if (idx >= this->size) {
                throw std::out_of_range("Index out of range");
            }
            return this->data[idx];
        }

        const T& operator[](size_t idx) const {
            if (idx >= this->size) {
                throw std::out_of_range("Index out of range");
            }
            return this->data[idx];
        }
};

struct Pair {
    std::string raw_str;
    uint64_t numeric_key;

    Pair() = default;

    explicit Pair(const std::string& str) : raw_str(str), numeric_key(0) {
        for (char c : raw_str) {
            if (c == '\t') {
                break;
            }
            if (c >= '0'&& c <= '9') {
                numeric_key = numeric_key * 10 + (c - '0');
            }
        }
    }

    ~Pair() = default;

    friend std::ostream& operator<<(std::ostream& os, const Pair& pair) {
        os << pair.raw_str;
        return os;
    }
};

void CountingSortStep(Vector<Pair>& input, Vector<Pair>& output, int shift) {
    Vector<int> extra(256, 0);

    for (size_t i = 0; i < input.Size(); i++) {
        uint8_t index = (input[i].numeric_key >> shift) & 0xFF;
        extra[index]++;
    }

    for (size_t i = 1; i < extra.Size(); i++) {
        extra[i] += extra[i - 1];
    }
    
    for (int i = (int)input.Size() - 1; i >= 0; i--) {
        uint8_t index = (input[i].numeric_key >> shift) & 0xFF;
        extra[index]--;
        output[extra[index]] = std::move(input[i]);
    }
}

void RadixSort(Vector<Pair>& pairs) {
    if (pairs.Size() < 2) return;

    Vector<Pair> buffer(pairs.Size());

    Vector<Pair>* src = &pairs;
    Vector<Pair>* dest = &buffer;

    for (int byte = 0; byte < 8; byte++) {
        CountingSortStep(*src, *dest, byte * 8);

        std::swap(src, dest);
    }

    if (src != &pairs) {
        pairs = std::move(*src);
    }
}


int main() {
    Vector<Pair> pairs;
    std::string input;

    while (std::getline(std::cin, input)) {
        if (input.empty()) continue;

        pairs.PushBack(Pair(input));
    }

    RadixSort(pairs);
    
    std::cout << pairs;
}
