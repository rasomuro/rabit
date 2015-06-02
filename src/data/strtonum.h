/*!
 *  Copyright (c) 2015 by Contributors
 * \file strtonum.h
 * \brief A faster implementation of strtod, ...
 */
#ifndef DMLC_DATA_STRTONUM_H_
#define DMLC_DATA_STRTONUM_H_
#include <cstdint>
#include "dmlc/base.h"

namespace dmlc {
namespace data {
inline bool isspace(char c) {
  return (c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\f');
}

inline bool isblank(char c) {
  return (c == ' ' || c == '\t');
}

inline bool isdigit(char c) {
  return (c >= '0' && c <= '9');
}

inline bool isdigits(char c) {
  return (c >= '0' && c <= '9') || c == '+' || c == '-' || c == '.';
}

/*!
 * \brief A faster version of strtof
 * TODO the current version does not support INF, NAN, and hex number
 */
inline float strtof(const char *nptr, char **endptr) {
  const char *p = nptr;
  // Skip leading white space, if any. Not necessary
  while (isspace(*p) ) ++ p;

  // Get sign, if any.
  bool sign = true;
  if (*p == '-') {
    sign = false; ++ p;
  } else if (*p == '+') {
    ++ p;
  }

  // Get digits before decimal point or exponent, if any.
  float value;
  for (value = 0; isdigit(*p); ++p) {
    value = value * 10.0 + (*p - '0');
  }

  // Get digits after decimal point, if any.
  if (*p == '.') {
    unsigned pow10 = 1;
    unsigned val2 = 0;
    ++ p;
    while (isdigit(*p)) {
      val2 = val2 * 10 + (*p - '0');
      pow10 *= 10;
      ++ p;
    }
    // std::cout << val2 << "  " << pow10 << std::endl;
    value += (float)val2 / (float)pow10;
  }

  // Handle exponent, if any.
  if ((*p == 'e') || (*p == 'E')) {
    ++ p;
    bool frac = false;
    float scale = 1.0;
    unsigned expon;
    // Get sign of exponent, if any.
    if (*p == '-') {
      frac = true;
      ++ p;
    } else if (*p == '+') {
      ++ p;
    }
    // Get digits of exponent, if any.
    for (expon = 0; isdigit(*p); p += 1) {
      expon = expon * 10 + (*p - '0');
    }
    if (expon > 38) expon = 38;
    // Calculate scaling factor.
    while (expon >=  8) { scale *= 1E8;  expon -=  8; }
    while (expon >   0) { scale *= 10.0; expon -=  1; }
    // Return signed and scaled floating point result.
    value = frac ? (value / scale) : (value * scale);
  }

  if (endptr) *endptr = (char*) p;
  return sign ? value : - value;
}

/**
 * \brief A faster string to integer convertor
 * TODO only support base <=10
 */
template <typename V>
inline V strtoint(const char* nptr, char **endptr, int base) {
  const char *p = nptr;
  // Skip leading white space, if any. Not necessary
  while (isspace(*p) ) ++ p;

  // Get sign if any
  bool sign = true;
  if (*p == '-') {
    sign = false; ++ p;
  } else if (*p == '+') {
    ++ p;
  }

  V value;
  for (value = 0; isdigit(*p); ++p) {
    value = value * base + (*p - '0');
  }

  if (endptr) *endptr = (char*) p;
  return sign ? value : - value;
}

inline uint64_t
strtoull(const char* nptr, char **endptr, int base) {
  return strtoint<uint64_t>(nptr, endptr, base);
};

inline long atol(const char* p) {
  return strtoint<long>(p, 0, 10);
}

inline float atof(const char *nptr) {
  return strtof(nptr, 0);
}


template<typename T>
class Str2T {
public:
  static inline T get(const char * begin, const char * end);
};

template<typename T>
inline T Str2Type(const char * begin, const char * end) {
  return Str2T<T>::get(begin, end);
}

template<>
class Str2T<int32_t> {
public:
  static inline int32_t get(const char * begin, const char * end) {
    return strtoint<int>(begin, NULL, 10);
  }
};

template<>
class Str2T<uint32_t> {
public:
  static inline uint32_t get(const char * begin, const char * end) {
    return strtoint<int>(begin, NULL, 10);
  }
};

template<>
class Str2T<int64_t> {
public:
  static inline int64_t get(const char * begin, const char * end) {
    return strtoint<int64_t>(begin, NULL, 10);
  }
};

template<>
class Str2T<uint64_t> {
public:
  static inline uint64_t get(const char * begin, const char * end) {
    return strtoint<uint64_t>(begin, NULL, 10);
  }
};

template<>
class Str2T<float> {
public:
  static inline float get(const char * begin, const char * end) {
    return atof(begin);
  }
};

/**
* \brief Parse colon seperated pair v1[:v2]
* \param begin: pointer to string
* \param end: one past end of string
* \param parseEnd: end string of parsed string
* \param v1: first value in the pair
* \param v2: second value in the pair
* \output number of values parsed
*/
template<typename T1, typename T2>
inline int ParsePair(const char * begin, const char * end, const char ** endptr, T1 &v1, T2 &v2) {
  const char * p = begin;
  while (p != end && !isdigits(*p)) ++p;
  if (p == end) {
    *endptr = end;
    return 0;
  }
  const char * q = p;
  while (q != end && isdigits(*q)) ++q;
  v1 = Str2Type<T1>(p, q);
  p = q;
  while (p != end && isblank(*p)) ++p;
  if (p == end || *p != ':') {
    // only v1
    *endptr = p;
    return 1;
  }
  p++;
  while (p != end && !isdigits(*p)) ++p;
  q = p;
  while (q != end && isdigits(*q)) ++q;
  *endptr = q;
  v2 = Str2Type<T2>(p, q);
  return 2;
}
}  // namespace data
}  // namespace dmlc
#endif /* DMLC_DATA_STRTONUM_H_ */
