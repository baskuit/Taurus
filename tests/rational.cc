#include <algorithm>
#include <iostream>
#include <cstring>
#include <stdint.h>

template <typename T = long unsigned int>
struct PrimeField
{

    static constexpr int bits = 8 * sizeof(T);

    T s0{0};
    T s1{0};

    PrimeField &operator*=(const PrimeField &other)
    {
        s0 ^= other.so;
        s1 = (s0 & other.s0);
        return (*this);
    }
};

template <typename U = int, int size = 8>
struct PrimeArray
{

    U data[size] = {0};
    PrimeArray &operator*=(const PrimeArray &other)
    {
        std::transform(data, data + size, other.data, data, [](int a, int b)
                       { return a + b; });
        return (*this);
    }
};

template <typename T, typename U, int size>
struct EEE
{
    static constexpr int tail_bits = 8 * sizeof(U);

    static uint8_t primes[size + tail_bits];

    static void set_primes(uint8_t *new_primes, int len)
    {
        memcpy(new_primes, EEE::primes, len);
    }

    PrimeArray<T, size> head;
    PrimeField<U> tail_num;
    PrimeField<U> tail_den;


    // constexpr EEE (T *data, int data_size, ) {

    // }/
};

template <typename T, typename U, int size>
uint8_t EEE<T, U, size>::primes[size + tail_bits] = {0};

int main()
{

    using EEE = EEE<int8_t, uint64_t, 8>;
    const int len = 54;
    uint8_t p[len] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251};
    EEE::set_primes(p, len);

    // EEE x{1};


    return 0;
}