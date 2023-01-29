#include <cassert>
#include <cstdio>
#include <cstring>
#include <cstdint>

template <int N>
int find_high_bits_in_byte_r(uint8_t x, int base_idx)
{
    uint8_t high = x >> N;
    uint8_t low = ((1 << N) - 1) & x;
    if (high > 0)
    {
        return find_high_bits_in_byte_r<N / 2>(high, base_idx + N);
    }
    else
    {
        return find_high_bits_in_byte_r<N / 2>(low, base_idx);
    }
}

template <>
int find_high_bits_in_byte_r<0>(uint8_t x, int base_idx)
{
    return base_idx;
}

int find_high_bits_in_byte(uint8_t x)
{
    return find_high_bits_in_byte_r<4>(x, 0);
}

template <int N>
bool is_greater_than_zero(uint8_t data[N])
{
    uint8_t zerofill[N] = { 0 };
    auto retval = std::memcmp(data, zerofill, N);
    return (retval != 0);
}

template <int N>
int find_high_bit_r(uint8_t data[N], int base_idx)
{
    union container_t {
        uint8_t data[N];
        struct
        {
            uint8_t low[N / 2];
            uint8_t high[N / 2];
        };
    } c;
    std::memcpy(c.data, data, N);

    if (is_greater_than_zero<N / 2>(c.high))
    {
        return find_high_bit_r<N / 2>(c.high, base_idx + N / 2);
    }
    else
    {
        return find_high_bit_r<N / 2>(c.low, base_idx);
    }
}

template <>
int find_high_bit_r<1>(uint8_t data[1], int base_idx)
{
    auto bit_idx = find_high_bits_in_byte(data[0]);
    return bit_idx + base_idx * 8;
}

template <typename T>
int find_high_bit(T x)
{
    if (x == 0)
    {
        return -1;
    }

    union container_t {
        T val;
        uint8_t data[sizeof(T)];
    };
    container_t c;
    c.val = x;
    return find_high_bit_r<sizeof(T)>(c.data, 0);
}

int main()
{
    for (int i = 0; i < 8; i++)
    {
        assert(i == find_high_bits_in_byte(1 << i));
    }

    for (int i = 0; i < 32; i++)
    {
        assert(i == find_high_bit<uint32_t>(1 << i));
    }

    assert(3 == find_high_bit<uint32_t>(0b1011));
    assert(27 == find_high_bit<uint32_t>(0b00001000'00000100'00000010'00000001));

    return 0;
}
