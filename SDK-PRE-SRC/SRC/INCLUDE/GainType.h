#ifndef _GAINTYPE_H
#define _GAINTYPE_H


#include <float.h>
#include <limits.h>

typedef long long GainType;
#ifndef LLONG_MAX
#define LLONG_MAX 9223372036854775807LL
#endif

#ifndef LLONG_MIN
#define LLONG_MIN (-LLONG_MAX - 1LL)
#endif

#define PLUS_INFINITY LLONG_MAX
#define MINUS_INFINITY LLONG_MIN
#define GainFormat "%lld"
#define GainInputFormat "%lld"

#endif
