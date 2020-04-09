#include "Ilog.hpp"
#if defined(__ARM_FEATURE_SIMD32) || defined(__ARM_NEON)
#define vreinterpretq_m128_f32(x) \
        (x)
#define vreinterpretq_f32_m128(x) \
        (x)
#include <arm_neon.h>
#endif

auto Ilog::log(uint16_t x) const -> int { return static_cast<int>(t[x]); }

Ilog *Ilog::mPInstance = nullptr;

auto Ilog::getInstance() -> Ilog * {
  if( mPInstance == nullptr ) {   // Only allow one getInstance of class to be generated.
    mPInstance = new Ilog;
  }

  return mPInstance;
}

Ilog *ilog = Ilog::getInstance();

auto llog(uint32_t x) -> int {
  if( x >= 0x1000000 ) {
    return 256 + ilog->log(uint16_t(x >> 16U));
  }
  if( x >= 0x10000 ) {
    return 128 + ilog->log(uint16_t(x >> 8U));
  }

  return ilog->log(uint16_t(x));
}

auto bitCount(uint32_t v) -> uint32_t {
  v -= ((v >> 1U) & 0x55555555U);
  v = ((v >> 2U) & 0x33333333U) + (v & 0x33333333U);
  v = ((v >> 4U) + v) & 0x0f0f0f0fU;
  v = ((v >> 8U) + v) & 0x00ff00ffU;
  v = ((v >> 16U) + v) & 0x0000ffffU;
  return v;
}

auto VLICost(uint64_t n) -> int {
  int cost = 1;
  while( n > 0x7F ) {
    n >>= 7U;
    cost++;
  }
  return cost;
}

auto rsqrt(const float x) -> float {
#if defined(__ARM_FEATURE_SIMD32) || defined(__ARM_NEON)
  float r = vgetq_lane_f32(vreinterpretq_f32_m128(vreinterpretq_m128_f32(vrsqrteq_f32(vreinterpretq_f32_m128(vdupq_n_f32(x))))),0); //NEON
#else
  float r = _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(x))); //SSE
#endif
  return (0.5F * (r + 1.0F / (x * r)));
}
