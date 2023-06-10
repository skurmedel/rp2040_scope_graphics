#pragma once

#include <cmath>
#include <limits>
#include <stdint.h>

/**
 * Routines for mathematical operations on lists of fixed point numbers.
 *
 * The main target here is the RP2040 which lacks an FPU. The bootrom floating
 * point routines are likely very quick, but not quick enough for the kind of
 * things I have in mind.
 *
 * The primary purpose for these routines are vector drawing, simple 3D graphics
 * and image output on an oscilloscope. As of 2023, most hobbyist DAC circuits
 * have a maximum precision of 12 bits.
 *
 * The primary format is Q16 stored in a signed 32-bit integer: a signed fixed
 * point number with 16 integer bits and 16 fractional bits. This allows us to
 * represent our DAC value either as a fraction or an integer, which is useful
 * depending on the application.
 *
 * THIS CODE ASSUMES TWO'S COMPLEMENT.
 */

namespace gfxlib {
template <typename TRaw = int32_t, unsigned char FracBits = 16>
struct FixedPoint {
    using raw_type = TRaw;
    using raw_limits = std::numeric_limits<raw_type>;
    static_assert(raw_limits::is_signed, "This code assumes a signed TRaw");

    static constexpr int fractional_bits = FracBits;
    static constexpr int integer_bits = raw_limits::digits - fractional_bits;
    static_assert(integer_bits >= 1, "fractional_bits are greater than the "
                                     "number of non sign bits in the type.");
    static constexpr int integer_bits_with_sign = integer_bits + 1;

    static constexpr raw_type max_integer = raw_type((1 << integer_bits) - 1);
    static constexpr raw_type min_integer = raw_type(-max_integer - 1);

    static constexpr raw_type max_fractional =
        raw_type((1 << fractional_bits) - 1);
    static constexpr raw_type min_fractional = raw_type(-max_fractional - 1);

    static constexpr raw_type sign_mask = (1 << raw_limits::digits);
    static constexpr raw_type integer_mask = ~((1 << fractional_bits) - 1);
    static constexpr raw_type fractional_mask =
        ((1 << fractional_bits) - 1) | sign_mask;

    static auto integer(raw_type &val) -> raw_type {
        return val >> fractional_bits;
    }

    static auto fractional(raw_type &val) -> raw_type {
        // TODO: this might be slowish, maybe we can do better with some intrinsics.
        raw_type sign = std::signbit(val)? sign_mask : 0;
        return ((val << integer_bits) | sign) >> integer_bits;
    }

    /**
     * Creates a new fixed point number with the integer part set to val. If val
     * cannot fit into the integer bits, this saturates.
     */
    static auto from_integer(raw_type val) -> raw_type {
        return from_parts(val, 0);
    }

    /**
     * Creates a new fixed point number. If any part is too large, this
     * saturates that part. It is not clever in any way, for example, if q16
     * then `from_parts(0, 65536)` will not be equivalent to 1.0, but 1/65535.
     *
     * If the signs differ for the parts, this assumes a negative sign; this
     * ensures that if either part is at negative saturation, it will always
     * fit.
     */
    static auto from_parts(raw_type integer, raw_type fractional) -> raw_type {
        raw_type n = 0;

        bool intsign = std::signbit(integer);
        bool fracsign = std::signbit(fractional);
        bool sign = intsign != fracsign ? true : intsign;

        // In two's complement, negating a positive number will not overflow.
        // TOOD: look at copying the sign instead.
        if (intsign != sign)
            integer = -integer;
        if (fracsign != sign)
            fractional = -fractional;

        if (integer > max_integer)
            n = max_integer;
        else if (integer < min_integer)
            n = min_integer;
        else
            n = integer;
        n <<= fractional_bits;

        if (fractional > max_fractional)
            n |= (max_fractional & (fractional_mask));
        else if (fractional < min_fractional)
            n |= (min_fractional & (fractional_mask));
        else
            n |= (fractional & (fractional_mask));
        return n;
    }

    /**
     * Saturating addition: a + b.
     */
    static auto sadd(raw_type a, raw_type b) -> raw_type;

    /**
     * Saturating subtraction: a - b.
     */
    static auto ssub(raw_type a, raw_type b) -> raw_type;

    /**
     * Saturating subtraction: a * b.
     */
    static auto smul(raw_type a, raw_type b) -> raw_type;
};

using q16_bits = int32_t;
using q16_traits = FixedPoint<q16_bits, 16>;

} // namespace gfxlib