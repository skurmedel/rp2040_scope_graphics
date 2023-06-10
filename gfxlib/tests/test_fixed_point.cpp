#include <catch2/catch_test_macros.hpp>

#include <gfxlib/fixed_point.hpp>

using namespace gfxlib;

struct BinaryCase {
    q16_bits a;
    q16_bits b;
    q16_bits expected;
};

TEST_CASE("q16_traits: from_parts") {
    q16_bits n;

    // INTEGER PARTS
    // Saturate upwards
    n = q16_traits::from_parts(1 << 15, 0);
    CHECK(n == ((1 << 15) - 1) << 16);
    CHECK(q16_traits::integer(n) == q16_traits::max_integer);

    // Saturate downwards
    n = q16_traits::from_parts(-(1 << 15) - 1, 0);
    CHECK(n == q16_bits(uint32_t(1 << 31)));
    CHECK(q16_traits::integer(n) == q16_traits::min_integer);

    // in range
    n = q16_traits::from_parts(5, 0);
    CHECK(n == (5 << 16));

    // FRACTIONAL PARTS
    n = q16_traits::from_parts(0, 1 << 16);
    CHECK(n == ((1 << 16) - 1));
    CHECK(q16_traits::fractional(n) == q16_traits::max_fractional);

    n = q16_traits::from_parts(0, -(1 << 16) - 1);
    CHECK(n == 1 << q16_traits::raw_limits::digits);
    CHECK(q16_traits::fractional(n) == q16_traits::min_fractional);

    // MIXED
    // different signs, negative assumed.
    n = q16_traits::from_parts(q16_traits::max_integer, q16_traits::min_fractional - 1);
    CHECK(q16_traits::integer(n) == -q16_traits::max_integer);
    CHECK(q16_traits::fractional(n) == q16_traits::min_fractional);

    n = q16_traits::from_parts(q16_traits::min_integer - 1, q16_traits::max_fractional);
    CHECK(q16_traits::integer(n) == q16_traits::min_integer);
    CHECK(q16_traits::fractional(n) == -q16_traits::max_fractional);
}

TEST_CASE("q16_traits: sadd") {}