#ifndef __UTIL_MATHS_HEADER_
#define __UTIL_MATHS_HEADER_
#include <math.h>

#include "common/util/Util.h"

//! Random between a and b (a and b included, a must be inferior to b)
#define UTIL_RANDOM(_a, _b) (((rand())*((_b) - (_a)))/(RAND_MAX) + (_a))

#ifndef ABS
	#define ABS(a) ((a) >= 0 ? (a) : (-(a)))
#endif

#ifndef MIN
	#define MIN(a,b) ((a) > (b) ? (b) : (a))
#endif

#ifndef MAX
	#define MAX(a,b) ((a) < (b) ? (b) : (a))
#endif

// _val must be in [_a, _b] included
#define CLAMP(_val, _a, _b) ((_val) < (_a) ? (_a) : MIN((_val), (_b)))

// if x < 0.5 return 0. else return 1.
#define ROUND(_val) floor((_val) + 0.5)
#define ROUND_DOWN_2(_val) floor(_val*100)/100;
#define ROUND_2(_val) ROUND(_val*100)/100;
#define ROUND_UP_2(_val) ceil(_val*100)/100;

#define APPROX_EQUAL(_val, _val2, _epsilon) ((_val) >= ((_val2) - (_epsilon)) && (_val) <= ((_val2) + (_epsilon)))

#define UTIL_NANO_EPSILON (1.e-307)
//#define UTIL_NANO_EPSILON (1.e-060)
#define UTIL_MICRO_EPSILON (1.e-150)
#define UTIL_EPSILON (1.e-010)
#define UTIL_SMALL_EPSILON (1.e-004)
#define UTIL_LARGE_EPSILON (1.e-001) //(1.e-005)
#define UTIL_PI (3.14159265358979323846)
//#define UTIL_PI (M_PI)
#define UTIL_LOG2E 1.44269504088896340736 // log2(exp(1))
//#define UTIL_LOG2E (M_LOG2E) // log2(exp(1))
#define UTIL_BIG_POSITIVE_NUMBER (1e+50)
#define UTIL_BIG_NEGATIVE_NUMBER (-1e+50)
#define UTIL_BIG_POSITIVE_U32_NUMBER (0xFFFFFFFF)
#define UTIL_BIG_POSITIVE_U16_NUMBER (0xFFFF)

#define RADIAN_TO_DEGREE(_x) (_x*360/(2*UTIL_PI))
#define DEGREE_TO_RADIAN(_x) (_x*2*UTIL_PI/360.)
//#define DEGREE_TO_RADIAN(_x) (_x*2*UTIL_PI/180.) // previous wrong formula used !-_-

#define UTIL_SQRT_TWO_PI (2.5066282746310002)
#define UTIL_GAUSSIAN_2(_x, _sigma) (exp(-((_x)*(_x))/(2*(_sigma)*(_sigma))))
#define UTIL_GAUSSIAN_3(_x, _sigma, _mu) (exp(-(((_x) - (_mu))*((_x) - (_mu)))/(2*(_sigma)*(_sigma))))
#define UTIL_GAUSSIAN_4(_x, _sigma, _mu, _psi) (exp(-(((_x)/(_psi) - (_mu))*((_x)/(_psi) - (_mu)))/(2*(_sigma)*(_sigma))))
#define UTIL_GAUSSIAN_5(_x, _sigma, _mu, _psi, _coeff) ((_coeff)*UTIL_GAUSSIAN_4(_x, _sigma, _mu, _psi))
#define UTIL_NORMALIZED_GAUSSIAN(x, _sigma, _mu, _psi) UTIL_GAUSSIAN_5(_x, _sigma, _mu, _psi, (1/((_sigma)*UTIL_SQRT_TWO_PI)))

//#define MIN_LOG_NUMBER (-1000000000)
#define MIN_LOG_NUMBER (-1.e+307)
inline q::qf64 specialLog(q::qf64 _x){
	if(APPROX_EQUAL(_x, 0., UTIL_NANO_EPSILON) == Q_TRUE){
		return MIN_LOG_NUMBER;	
	}
	return MAX(log(_x), MIN_LOG_NUMBER);
	//return MAX(log10(_x), MIN_LOG_NUMBER);
}

// REFS code from http://www.christophlassner.de/collection-of-msvc-gcc-compatibility-tricks.html
// "'isnan' and 'isinf', both functions do have a highly efficient version in MSVC. They have been forgotten to be defined as part of std in VS2012, but are included in VS2013."
#if defined(_MSC_VER)
	#if _MSC_VER < 1800
namespace std {
  template <typename T>
  bool isnan(const T &_x) { return _isnan(_x); }

  template <typename T>
  bool isinf(const T &_x) { return !_finite(_x); }
}
	#endif
#endif

#ifdef NEED_LOG_FUNCTIONS
	#include "thirdParties/numpy/npy_math.h"
#endif

#endif
