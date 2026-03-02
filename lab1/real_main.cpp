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
                new_data[i] = this->data[i];
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

        void PushBack(T value) {
            if (this->size == this->capacity) {
                size_t new_capacity;
                if (this->capacity == 0) {
                    new_capacity = 1;
                } else {
                    new_capacity = this->capacity * 2;
                }
                reallocate(new_capacity);
            }
            this->data[this->size] = value;
            this->size++;
        }

        void Pop(int idx = 0) {
            if (idx < 0 || idx >= this->size) {
                throw std::out_of_range("Index out of range");
            }
            for (size_t i = idx; i < this->size - 1; i++) {
                this->data[i] = this->data[i + 1];
            }
            this->size--;
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
    std::string key;
    std::string value;
    uint64_t numeric_key;

    Pair() = default;

    explicit Pair(const std::string& str) : numeric_key(0) {
        size_t delimiterPos = str.find('\t');

        if (delimiterPos == std::string::npos) {
            throw std::invalid_argument("Invalid input format. Expected 'key\tvalue'");
        }

        key = str.substr(0, delimiterPos);

        for (char c : key) {
            if (c >= '0'&& c <= '9') {
                numeric_key = numeric_key * 10 + (c - '0');
            }
        }

        size_t valueStart = str.find_first_not_of('\t', delimiterPos);
        if (valueStart != std::string::npos) {
            value = str.substr(valueStart);
        }
    }

    ~Pair() = default;

    friend std::ostream& operator<<(std::ostream& os, const Pair& pair) {
        os << pair.key << "\t" << pair.value;
        return os;
    }
};

Vector<Pair> CountingSort(const Vector<Pair>& pairs, int dig) {
    Vector<int> extra(256, 0);
    Vector<Pair> output(pairs.Size());

    for (int i = 0; i < pairs.Size(); i++) {
        uint8_t index = (pairs[i].numeric_key >> dig) & 0xFF;
        extra[index]++;
    }

    for (int i = 1; i < extra.Size(); i++) {
        extra[i] += extra[i - 1];
    }
    
    for (int i = pairs.Size() - 1; i >= 0; i--) {
        uint8_t index = (pairs[i].numeric_key >> dig) & 0xFF;
        extra[index]--;
        output[extra[index]] = pairs[i];
    }
    return output;
}


Vector<Pair> RadixSort(const Vector<Pair>& pairs) {
    Vector<Pair> output = pairs;

    for (int dig = 0; dig < 8; dig++) {
        output = CountingSort(output, dig * 8);
    }

    return output;
}


int main() {
    Vector<Pair> pairs;
    std::string input;
    //std::cout << "Enter a key-value pairs (key value):\n";

    while (std::getline(std::cin, input)) {
        if (input.empty()) continue;

        Pair pair = Pair(input);
        pairs.PushBack(pair);
    }

    pairs = RadixSort(pairs);

    //std::cout << "Sorted key-value pairs:\n";
    
    std::cout << pairs;
}
