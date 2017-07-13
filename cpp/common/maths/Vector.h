#ifndef __UTIL_VECTOR_HEADER_
#define __UTIL_VECTOR_HEADER_
#include "common/util/Util.h"
#include "common/maths/Maths.h"

#include "math.h"
 
#ifdef Q_SSE_AVX
	#if defined(__GNUC__)
		// #include <mmintrin.h> // MMX
		// #include <xmmintrin.h> // SSE
		// #include <emmintrin.h> // SSE2
		// #include <pmmintrin.h> // SSE3
		// #include <tmmintrin.h> // SSSE3
		// #include <smmintrin.h> // SSE4.1
		// #include <nmmintrin.h> // SSE4.2
		// #include <ammintrin.h> // SSE4A
		// #include <wmmintrin.h> // AES
		#include <immintrin.h> // AVX
		// #include <zmmintrin.h> // AVX512
	#endif
	
	#define Q_SSE_AVX_VECTOR
	#ifdef Q_SSE_AVX_VECTOR
		//#define Q_PURELY_DATA
	#endif
#endif

BEGIN_Q_NAMESPACE
	enum{
		AXE_X = 0,
		AXE_Y = 1,
		AXE_Z,
		AXE_COUNT
	};

	template<typename _TYPE>
	class TVector2
	{
	public:
		inline TVector2<_TYPE>(void)
		{
			m_Values[0] = 0;
			m_Values[1] = 0;
		}

		inline TVector2<_TYPE>(_TYPE _val1, _TYPE _val2)
		{
			m_Values[0] = _val1;
			m_Values[1] = _val2;
		}

		inline const _TYPE& operator[](const q::qu8 _i) const
		{
			qAssert(_i >= 0 && _i < 2);
			return m_Values[_i];
		}

		inline _TYPE& operator[](const q::qu8 _i)
		{
			qAssert(_i >= 0 && _i < 2);
			return m_Values[_i];
		}

		inline const TVector2<_TYPE> operator+(const TVector2<_TYPE> &_v) const{
			return TVector2<_TYPE>(m_Values[0] + _v.m_Values[0], m_Values[1] + _v.m_Values[1]);
		}

		inline const TVector2<_TYPE>& operator+=(const TVector2<_TYPE> &_v){
			for(qu32 i = 0; i < 2; ++i){
				m_Values[i] += _v.m_Values[i];
			}
			return *this;
		}

		inline const TVector2<_TYPE> operator-(void) const{
			return TVector2<_TYPE>(-m_Values[0], -m_Values[1]);
		}

		inline const TVector2<_TYPE> operator-(const TVector2<_TYPE> &_v) const{
			return TVector2<_TYPE>(m_Values[0] - _v.m_Values[0], m_Values[1] - _v.m_Values[1]);
		}

		inline const TVector2<_TYPE>& operator-=(const TVector2<_TYPE> &_v){
			for(qu32 i = 0; i < 2; ++i){
				m_Values[i] -= _v.m_Values[i];
			}
			return *this;
		}

		inline _TYPE norm2(void) const {
			return sqrt(m_Values[0]*m_Values[0] + m_Values[1]*m_Values[1]);
		}

		inline _TYPE length(void) const {
			return norm2();
		}

		inline void normalize(void){
			_TYPE norm = norm2();
			for(qu32 i = 0; i < 2; ++i){
				m_Values[i] /= norm;
			}
		}

		inline _TYPE dot(const TVector2<_TYPE> &_v) const {
			return m_Values[0]*_v.m_Values[0] + m_Values[1]*_v.m_Values[1];
		}

	//private:
	public:
		union{
			struct {
				_TYPE x;
				_TYPE y;
			};
			_TYPE m_Values[2];
		};
	};

	template <class _TYPE>
	inline TVector2<_TYPE> operator*(_TYPE _scalar, const TVector2<_TYPE> &_v){
		return TVector2<_TYPE>(_v[0]*_scalar, _v[1]*_scalar);
	}

	typedef TVector2<q::qf64> Vector2;

	template<typename _TYPE>
	class TVector3
	{
	public:
		inline TVector3<_TYPE>(void)
		{
			m_Values[0] = 0;
			m_Values[1] = 0;
			m_Values[2] = 0;
		}

		/*inline TVector3<_TYPE>(_TYPE _val)
		{
			m_Values[0] = _val;
			m_Values[1] = _val;
			m_Values[2] = _val;
		}*/

		inline TVector3<_TYPE>(_TYPE _val1, _TYPE _val2, _TYPE _val3)
		{
			m_Values[0] = _val1;
			m_Values[1] = _val2;
			m_Values[2] = _val3;
		}

		inline const _TYPE& operator[](const q::qu8 _i) const
		{
			qAssert(_i >= 0 && _i < 3);
			return m_Values[_i];
		}

		inline _TYPE& operator[](const q::qu8 _i)
		{
			qAssert(_i >= 0 && _i < 3);
			return m_Values[_i];
		}

		inline qbool operator<(const TVector3<_TYPE> &_v2) const{
			return m_Values[0] < _v2.m_Values[0] && m_Values[1] < _v2.m_Values[1] && m_Values[2] < _v2.m_Values[2];
		}

		inline const TVector3<_TYPE> operator-(void) const{
			return TVector3<_TYPE>(-m_Values[0], -m_Values[1], -m_Values[2]);
		}

		inline const TVector3<_TYPE> operator+(const TVector3<_TYPE> &_v) const{
			return TVector3<_TYPE>(m_Values[0] + _v.m_Values[0], m_Values[1] + _v.m_Values[1], m_Values[2] + _v.m_Values[2]);
		}

		inline const TVector3<_TYPE>& operator+=(const TVector3<_TYPE> &_v){
			for(qu32 i = 0; i < 3; ++i){
				m_Values[i] += _v.m_Values[i];
			}
			return *this;
		}

		inline const TVector3<_TYPE> operator-(const TVector3<_TYPE> &_v) const{
			return TVector3<_TYPE>(m_Values[0] - _v.m_Values[0], m_Values[1] - _v.m_Values[1], m_Values[2] - _v.m_Values[2]);
		}

		inline const TVector3<_TYPE>& operator-=(const TVector3<_TYPE> &_v){
			for(qu32 i = 0; i < 3; ++i){
				m_Values[i] -= _v.m_Values[i];
			}
			return *this;
		}

		inline const TVector3<_TYPE>& operator*=(_TYPE _scalar){
			for(qu32 i = 0; i < 3; ++i){
				m_Values[i] *= _scalar;
			}
			return *this;
		}

		inline _TYPE lengthSquared(void) const {
			return m_Values[0]*m_Values[0] + m_Values[1]*m_Values[1] + m_Values[2]*m_Values[2];
		}

		inline _TYPE norm2(void) const {
			return sqrt(lengthSquared());
		}

		inline _TYPE length(void) const {
			return norm2();
		}

		inline void normalize(void){
			_TYPE norm = norm2();
			for(qu32 i = 0; i < 3; ++i){
				m_Values[i] /= norm;
			}
		}

		inline _TYPE dot(const TVector3<_TYPE> _v) const {
			return m_Values[0]*_v.m_Values[0] + m_Values[1]*_v.m_Values[1] + m_Values[2]*_v.m_Values[2];
		}

		inline const TVector3<_TYPE> cross(const TVector3<_TYPE> _v) const{
			return TVector3<_TYPE>(m_Values[1]*_v.m_Values[2] - m_Values[2]*_v.m_Values[1]
									, m_Values[2]*_v.m_Values[0] - m_Values[0]*_v.m_Values[2]
									, m_Values[0]*_v.m_Values[1] - m_Values[1]*_v.m_Values[0]);
		}

	//private:
	public:
		union{
			struct {
				_TYPE x;
				_TYPE y;
				_TYPE z;
			};
			_TYPE m_Values[3];
		};
	};

	template <class _TYPE>
	inline TVector3<_TYPE> operator*(_TYPE _scalar, const TVector3<_TYPE> &_v){
		return TVector3<_TYPE>(_v[0]*_scalar, _v[1]*_scalar, _v[2]*_scalar);
	}

	template <class _TYPE>
	inline TVector3<_TYPE> operator*(const TVector3<_TYPE> &_v, _TYPE _scalar){
		return TVector3<_TYPE>(_v[0]*_scalar, _v[1]*_scalar, _v[2]*_scalar);
	}

	typedef TVector3<q::qf64> Vector3;
	typedef TVector3<q::qu32> Vector3qu32;
	typedef TVector3<q::qs32> Vector3qs32;
	
	template<typename _TYPE>
	class TVector4
	{
	public:
		inline TVector4<_TYPE>(void)
		{
			m_Values[0] = 0;
			m_Values[1] = 0;
			m_Values[2] = 0;
			m_Values[3] = 0;
		}

		inline TVector4<_TYPE>(_TYPE _val1)
		{
			m_Values[0] = _val1;
			m_Values[1] = _val1;
			m_Values[2] = _val1;
			m_Values[3] = _val1;
		}

		inline TVector4<_TYPE>(_TYPE _val1, _TYPE _val2, _TYPE _val3, _TYPE _val4)
		{
			m_Values[0] = _val1;
			m_Values[1] = _val2;
			m_Values[2] = _val3;
			m_Values[3] = _val4;
		}

		inline const _TYPE& operator[](const q::qu8 _i) const
		{
			qAssert(_i >= 0 && _i < 4);
			return m_Values[_i];
		}

		inline _TYPE& operator[](const q::qu8 _i)
		{
			qAssert(_i >= 0 && _i < 4);
			return m_Values[_i];
		}

		inline TVector4<_TYPE> operator*(const TVector4<_TYPE> &_v) const {
			return TVector4<_TYPE>(m_Values[0]*_v.m_Values[0], m_Values[1]*_v.m_Values[1], m_Values[2]*_v.m_Values[2], m_Values[3]*_v.m_Values[3]);
		}

		inline TVector4<_TYPE> operator+(const TVector4<_TYPE> &_v) const {
			return TVector4<_TYPE>(m_Values[0] + _v.m_Values[0], m_Values[1] + _v.m_Values[1], m_Values[2] + _v.m_Values[2], m_Values[3] + _v.m_Values[3]);
		}

		inline TVector4<_TYPE> operator-(const TVector4<_TYPE> &_v) const {
			return TVector4<_TYPE>(m_Values[0] - _v.m_Values[0], m_Values[1] - _v.m_Values[1], m_Values[2] - _v.m_Values[2], m_Values[3] - _v.m_Values[3]);
		}

		inline TVector4<_TYPE> operator/(const TVector4<_TYPE> &_v) const {
			return TVector4<_TYPE>(m_Values[0]/_v.m_Values[0], m_Values[1]/_v.m_Values[1], m_Values[2]/_v.m_Values[2], m_Values[3]/_v.m_Values[3]);
		}

		inline const TVector4<_TYPE>& operator*=(const TVector4<_TYPE> &_v){
			for(qu32 i = 0; i < 4; ++i){
				m_Values[i] *= _v.m_Values[i];
			}
			return *this;
		}

		inline const TVector4<_TYPE>& operator+=(const TVector4<_TYPE> &_v){
			for(qu32 i = 0; i < 4; ++i){
				m_Values[i] += _v.m_Values[i];
			}
			return *this;
		}

		inline const TVector4<_TYPE>& operator-=(const TVector4<_TYPE> &_v){
			for(qu32 i = 0; i < 4; ++i){
				m_Values[i] -= _v.m_Values[i];
			}
			return *this;
		}

		inline const TVector4<_TYPE>& operator/=(const TVector4<_TYPE> &_v){
			for(qu32 i = 0; i < 4; ++i){
				m_Values[i] /= _v.m_Values[i];
			}
			return *this;
		}

		inline const TVector4<_TYPE>& operator/=(_TYPE _scalar){
			for(qu32 i = 0; i < 4; ++i){
				m_Values[i] /= _scalar;
			}
			return *this;
		}

		inline _TYPE lengthSquared(void) const {
			return m_Values[0]*m_Values[0] + m_Values[1]*m_Values[1] + m_Values[2]*m_Values[2] + m_Values[3]*m_Values[3];
		}

		inline _TYPE norm2(void) const {
			return sqrt(lengthSquared());
		}

		inline _TYPE length(void) const {
			return norm2();
		}

		inline void normalize(void){
			_TYPE norm = norm2();
			for(qu32 i = 0; i < 4; ++i){
				m_Values[i] /= norm;
			}
		}

	private:
		_TYPE m_Values[4];
	};

#ifdef Q_SSE_AVX_VECTOR
	#ifdef Q_PURELY_DATA
	// REFS code inspired by http://www.gamasutra.com/view/feature/132636/designing_fast_crossplatform_simd_.php
	#else
	// REFS code inspired by fhtr.blogspot.nl/2010/02/4x4-float-matrix-multiplication-using.html
	template<>
	class TVector4 <q::qf64>
	{
	public:
		inline TVector4(void){
			m_Value = _mm256_set1_pd(0.);
		}

		inline TVector4(__m256d _v) : m_Value(_v){
		}

		inline TVector4(q::qf64 _val1){
			m_Value = _mm256_set1_pd(_val1);
		}

		inline TVector4(q::qf64 _val1, q::qf64 _val2, q::qf64 _val3, q::qf64 _val4){
			m_Value = _mm256_set_pd(_val4, _val3, _val2, _val1);
		}

		inline const q::qf64& operator[](const q::qu8 _i) const {
			qAssert(_i >= 0 && _i < 4);
			return m_Values[_i];
		}

		inline q::qf64& operator[](const q::qu8 _i){
			qAssert(_i >= 0 && _i < 4);
			return m_Values[_i];
		}

		inline TVector4 operator*(const TVector4 &_v) const {
			return TVector4(_mm256_mul_pd(m_Value, _v.m_Value));
		}

		inline TVector4 operator+(const TVector4 &_v) const {
			return TVector4(_mm256_add_pd(m_Value, _v.m_Value));
		}

		inline TVector4 operator-(const TVector4 &_v) const {
			return TVector4(_mm256_sub_pd(m_Value, _v.m_Value));
		}

		inline TVector4 operator/(const TVector4 &_v) const {
			return TVector4(_mm256_div_pd(m_Value, _v.m_Value));
		}

		inline const TVector4& operator*=(const TVector4 &_v){
			m_Value = _mm256_mul_pd(m_Value, _v.m_Value);
			return *this;
		}

		inline const TVector4& operator+=(const TVector4 &_v){
			m_Value = _mm256_add_pd(m_Value, _v.m_Value);
			return *this;
		}

		inline const TVector4& operator-=(const TVector4 &_v){
			m_Value = _mm256_sub_pd(m_Value, _v.m_Value);
			return *this;
		}

		inline const TVector4& operator/=(const TVector4 &_v){
			m_Value = _mm256_div_pd(m_Value, _v.m_Value);
			return *this;
		}

		inline const TVector4& operator/=(q::qf64 _scalar){
			__m256d scalar = _mm256_set1_pd(_scalar);
			m_Value = _mm256_div_pd(m_Value, scalar);
			return *this;
		}

		inline TVector4 operator>>(q::qf64 *_v) const {
			_mm256_store_pd(_v, m_Value);
		}

		inline q::qf64 lengthSquared(void) const {
			__m256d tmp = _mm256_mul_pd(m_Value, m_Value);
			return ((q::qf64*)&tmp)[0] + ((q::qf64*)&tmp)[1] + ((q::qf64*)&tmp)[2] + ((q::qf64*)&tmp)[3];
		}

		inline q::qf64 norm2(void) const {
			return sqrt(lengthSquared());
		}

		inline q::qf64 length(void) const {
			return norm2();
		}

		inline void normalize(void){
			__m256d norm = _mm256_set1_pd(norm2());
			_mm256_div_pd(m_Value, norm);
		}

	public:
		union {
			__m256d m_Value;
			q::qf64 m_Values[4];
		};
	};
	#endif
#endif

	template <class _TYPE>
	inline TVector4<_TYPE> operator*(_TYPE _scalar, const TVector4<_TYPE> &_v){
		return TVector4<_TYPE>(_v[0]*_scalar, _v[1]*_scalar, _v[2]*_scalar, _v[3]*_scalar);
	}

#ifdef Q_SSE_AVX_VECTOR
	template<>
	inline TVector4<q::qf64> operator*(q::qf64 _scalar, const TVector4<q::qf64> &_v){
		return TVector4<q::qf64>(_mm256_mul_pd(_mm256_set1_pd(_scalar), _v.m_Value));
	}
#endif

	typedef TVector4<q::qf64> Vector4;

	template<typename _TYPE>
	class TVector6
	{
	public:
		inline TVector6<_TYPE>(void)
		{
			for(qu8 i = 0; i < 6; ++i){
				m_Values[i] = 0.;
			}
		}

		inline TVector6<_TYPE>(_TYPE _val1, _TYPE _val2, _TYPE _val3, _TYPE _val4, _TYPE _val5, _TYPE _val6)
		{
			m_Values[0] = _val1;
			m_Values[1] = _val2;
			m_Values[2] = _val3;
			m_Values[3] = _val4;
			m_Values[4] = _val5;
			m_Values[5] = _val6;
		}

		inline const _TYPE& operator[](const q::qu8 _i) const
		{
			qAssert(_i >= 0 && _i < 6);
			return m_Values[_i];
		}

		inline _TYPE& operator[](const q::qu8 _i)
		{
			qAssert(_i >= 0 && _i < 6);
			return m_Values[_i];
		}

		inline const TVector6<_TYPE> operator+(const TVector6<_TYPE> &_v) const{
			return TVector6<_TYPE>(m_Values[0] + _v.m_Values[0], m_Values[1] + _v.m_Values[1], m_Values[2] + _v.m_Values[2]
								, m_Values[3] + _v.m_Values[3], m_Values[4] + _v.m_Values[4], m_Values[5] + _v.m_Values[5]);
		}

		inline const TVector6<_TYPE>& operator+=(const TVector6<_TYPE> &_v){
			for(qu32 i = 0; i < 6; ++i){
				m_Values[i] += _v.m_Values[i];
			}
			return *this;
		}

		inline const TVector6<_TYPE> operator-(const TVector6<_TYPE> &_v) const{
			return TVector6<_TYPE>(m_Values[0] - _v.m_Values[0], m_Values[1] - _v.m_Values[1], m_Values[2] - _v.m_Values[2]
								,m_Values[3] - _v.m_Values[3], m_Values[4] - _v.m_Values[4], m_Values[5] - _v.m_Values[5]);
		}

		inline const TVector6<_TYPE>& operator-=(const TVector6<_TYPE> &_v){
			for(qu32 i = 0; i < 6; ++i){
				m_Values[i] -= _v.m_Values[i];
			}
			return *this;
		}

		inline _TYPE lengthSquared(void) const {
			return m_Values[0]*m_Values[0] + m_Values[1]*m_Values[1] + m_Values[2]*m_Values[2]
				+ m_Values[3]*m_Values[3] + m_Values[4]*m_Values[4] + m_Values[5]*m_Values[5];
		}

		inline _TYPE norm2(void) const {
			return sqrt(lengthSquared());
		}

		inline _TYPE length(void) const {
			return norm2();
		}

		inline const TVector2<_TYPE> getVec2() const
		{
			return TVector2<_TYPE>(TVector6<_TYPE>::m_Values[0],
				TVector6<_TYPE>::m_Values[1]);
		}

		inline const TVector3<_TYPE> getVec3() const
		{
			return TVector3<_TYPE>(TVector6<_TYPE>::m_Values[0], 
				TVector6<_TYPE>::m_Values[1], 
				TVector6<_TYPE>::m_Values[2]);
		}

		inline const TVector4<_TYPE> getVec4() const
		{
			return TVector4<_TYPE>(TVector6<_TYPE>::m_Values[0], 
				TVector6<_TYPE>::m_Values[1], 
				TVector6<_TYPE>::m_Values[2], 
				TVector6<_TYPE>::m_Values[3]);
		}

	private:
		_TYPE m_Values[6];
	};

	typedef TVector6<q::qf64> Vector6;

	inline q::qf64 GetSquareDistance(const Vector2 &_vec1, const Vector2 &_vec2)
	{
		return (_vec2[0] - _vec1[0])*(_vec2[0] - _vec1[0])
			 + (_vec2[1] - _vec1[1])*(_vec2[1] - _vec1[1]);
	}

	inline q::qf64 GetDistance(const Vector2 &_vec1, const Vector2 &_vec2)
	{
		return sqrt(GetSquareDistance(_vec1, _vec2));
	}

	inline q::qf64 GetAbsoluteDistance(const Vector2 &_vec1, const Vector2 &_vec2)
	{
		return ABS(_vec2[0] - _vec1[0])
			 + ABS(_vec2[1] - _vec1[1]);
	}

	inline q::qf64 GetSquareDistance(const Vector3 &_vec1, const Vector3 &_vec2)
	{
		return (_vec2[0] - _vec1[0])*(_vec2[0] - _vec1[0])
			 + (_vec2[1] - _vec1[1])*(_vec2[1] - _vec1[1])
			 + (_vec2[2] - _vec1[2])*(_vec2[2] - _vec1[2]);
	}

	inline q::qf64 GetDistance(const Vector3 &_vec1, const Vector3 &_vec2)
	{
		return sqrt(GetSquareDistance(_vec1, _vec2));
	}

	inline q::qf64 GetSquareDistance(const Vector4 &_vec1, const Vector4 &_vec2)
	{
		return (_vec2[0] - _vec1[0])*(_vec2[0] - _vec1[0])
			 + (_vec2[1] - _vec1[1])*(_vec2[1] - _vec1[1])
			 + (_vec2[2] - _vec1[2])*(_vec2[2] - _vec1[2])
			 /*+ (_vec2[3] - _vec1[3])*(_vec2[3] - _vec1[3])*/;
	}

	inline q::qf64 GetDistance(const Vector4 &_vec1, const Vector4 &_vec2)
	{
		return sqrt(GetSquareDistance(_vec1, _vec2));
	}

	inline q::qf64 GetAbsoluteDistance(const Vector4 &_vec1, const Vector4 &_vec2)
	{
		return ABS(_vec2[0] - _vec1[0])
			 + ABS(_vec2[1] - _vec1[1])
			 + ABS(_vec2[2] - _vec1[2]);
	}

	inline q::qf64 GetSquareDistance(const Vector6 &_vec1, const Vector6 &_vec2)
	{
		return (_vec2[0] - _vec1[0])*(_vec2[0] - _vec1[0])
			 + (_vec2[1] - _vec1[1])*(_vec2[1] - _vec1[1])
			 + (_vec2[2] - _vec1[2])*(_vec2[2] - _vec1[2])
			 + (_vec2[3] - _vec1[3])*(_vec2[3] - _vec1[3])
			 /*+ (_vec2[4] - _vec1[4])*(_vec2[4] - _vec1[4])
			 + (_vec2[5] - _vec1[5])*(_vec2[5] - _vec1[5])*/;
	}

	inline q::qf64 GetDistance(const Vector6 &_vec1, const Vector6 &_vec2){
		return sqrt(GetSquareDistance(_vec1, _vec2));
	}

	inline Vector3 GetVector3(const Vector4 &_vec){
		return Vector3(_vec[0], _vec[1], _vec[2]);
	}

	inline Vector4 GetVector4(const Vector3 &_vec){
		return Vector4(_vec[0], _vec[1], _vec[2], 1.0);
	}

	inline Vector3 RoundVector3(const Vector3 &_vec){
		return Vector3(ROUND(_vec[0]), ROUND(_vec[1]), ROUND(_vec[2]));
	}

	inline qbool ApproxEqual(const Vector6 &_v1, const Vector6 &_v2){
		for(qu32 i = 0; i < 6; ++i){
			if(APPROX_EQUAL(_v1[i], _v2[i], UTIL_EPSILON) == Q_FALSE){
				return Q_FALSE;
			}
		}
		return Q_TRUE;
	}

	inline void PrintVector2(const Vector2 &_vec)
	{
		qPrint("%f %f\n", _vec[0], _vec[1]);
	}

	inline void PrintVector3(const Vector3 &_vec)
	{
		qPrint("%f %f %f\n", _vec[0], _vec[1], _vec[2]);
	}

	inline void PrintVector4(const Vector4 &_vec)
	{
		qPrint("%f %f %f %f\n", _vec[0], _vec[1], _vec[2], _vec[3]);
	}

END_Q_NAMESPACE

#endif
