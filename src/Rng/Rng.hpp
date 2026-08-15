#pragma once

#include <ctime>

#define LCG_MAX (0xFFFFFFFF);

class SimpleRng {
  unsigned long long a;

public:
  SimpleRng() { a = (unsigned long long)time(0); }
  inline unsigned int LCG() {
    a = (a * 25214903917 + 11) & 0xFFFFFFFFFFFF;
    return a >> 16;
  }
  inline int GetIntRange(int min, int max) { return (LCG() % (max + 1)) + min; }
  inline double GetDoubleUnif() { return (double)LCG() / (double)LCG_MAX; }
};