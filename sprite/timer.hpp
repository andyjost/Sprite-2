/**
 * @file
 * @brief A cycle timer specific to x86-64 architecture.
 */
#pragma once

namespace sprite
{
  typedef unsigned long long int ticks;
  static inline ticks getticks()
  {
    unsigned int a, d;
    asm volatile("rdtsc" : "=a" (a), "=d" (d)); // x86-64
    return ((ticks)a) | (((ticks)d) << 32);
  }
}

