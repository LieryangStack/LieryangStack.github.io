#include "MathFunctions.h"

#include <cmath>
#include <iostream>

#ifdef USE_MYMATH
#  include "mysqrt.h"
#endif

namespace mathfunctions {
  double sqrt(double x) {
  // which square root function should we use?
#ifdef USE_MYMATH
    std::cout << "USE_MYMATH = " << USE_MYMATH << std::endl;
    return detail::mysqrt(x);
#else
    return std::sqrt(x);
#endif
  }
}
