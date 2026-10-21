#include <platform/simd.hpp>
#include <platform/simd.hpp> // pragma once

#include <iostream>

namespace {

// Architecture invariants.  These catch a broken detector even when the
// exact feature set depends on the machine.
bool consistent(const simd::features& f)
{
    if (f.sse3 && !f.sse2)
        return false;
    if (f.ssse3 && !f.sse3)
        return false;
    if (f.sse4_1 && !f.ssse3)
        return false;
    if (f.sse4_2 && !f.sse4_1)
        return false;
    if (f.avx2 && !f.avx)
        return false;
    if (f.avx512f && !f.avx2)
        return false;

    if (f.sve && !f.neon)
        return false;
    if (f.sve2 && !f.sve)
        return false;
    if (f.dotprod && !f.neon)
        return false;
    if (f.fp16 && !f.neon)
        return false;
    if (f.i8mm && !f.neon)
        return false;
    if (f.bf16 && !f.neon)
        return false;

    return true;
}

} // namespace

int main()
{
    const simd::features& f = simd::cached();

    std::cout << std::boolalpha
              << "sse2       " << f.sse2 << '\n'
              << "sse3       " << f.sse3 << '\n'
              << "ssse3      " << f.ssse3 << '\n'
              << "sse4.1     " << f.sse4_1 << '\n'
              << "sse4.2     " << f.sse4_2 << '\n'
              << "avx        " << f.avx << '\n'
              << "avx2       " << f.avx2 << '\n'
              << "avx512f    " << f.avx512f << '\n'
              << "neon       " << f.neon << '\n'
              << "sve        " << f.sve << '\n'
              << "sve2       " << f.sve2 << '\n'
              << "dotprod    " << f.dotprod << '\n'
              << "fp16       " << f.fp16 << '\n'
              << "i8mm       " << f.i8mm << '\n'
              << "bf16       " << f.bf16 << '\n'
              << "list       " << simd::to_string(f) << '\n';

    if (!consistent(f)) {
        std::cerr << "inconsistent SIMD feature set\n";
        return 1;
    }
    return 0;
}
