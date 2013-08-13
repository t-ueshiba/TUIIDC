/*
 *  平成14-19年（独）産業技術総合研究所 著作権所有
 *  
 *  創作者：植芝俊夫
 *
 *  本プログラムは（独）産業技術総合研究所の職員である植芝俊夫が創作し，
 *  （独）産業技術総合研究所が著作権を所有する秘密情報です．著作権所有
 *  者による許可なしに本プログラムを使用，複製，改変，第三者へ開示する
 *  等の行為を禁止します．
 *  
 *  このプログラムによって生じるいかなる損害に対しても，著作権所有者お
 *  よび創作者は責任を負いません。
 *
 *  Copyright 2002-2007.
 *  National Institute of Advanced Industrial Science and Technology (AIST)
 *
 *  Creator: Toshio UESHIBA
 *
 *  [AIST Confidential and all rights reserved.]
 *  This program is confidential. Any using, copying, changing or
 *  giving any information concerning with this program to others
 *  without permission by the copyright holder are strictly prohibited.
 *
 *  [No Warranty.]
 *  The copyright holder or the creator are not responsible for any
 *  damages caused by using this program.
 *  
 *  $Id$
 */
/*!
  \file		mmInstructions.h
  \brief	Intel CPUのマルチメディア命令に関連するクラスと関数の定義と実装
*/
#if !defined(__mmInstructions_h) && defined(__INTEL_COMPILER)
#define __mmInstructions_h

#include <iostream>
#include <cassert>
#include <boost/iterator_adaptors.hpp>
#include "TU/types.h"

#if defined(AVX2)		// Haswell (2013?)
#  define AVX
#endif
#if defined(AVX)		// Core-i7 Sandy-Bridge (2011)
#  define SSE4
#endif
#if defined(SSE4)		// Core2 with Penryn core(45nm)
#  define SSSE3
#endif
#if defined(SSSE3)		// Core2 (Jun. 2006)
#  define SSE3
#endif
#if defined(SSE3)		// Pentium-4 with Prescott core (Feb. 2004)
#  define SSE2
#endif
#if defined(SSE2)		// Pentium-4 (Nov. 2000)
#  define SSE
#endif
#if defined(SSE)		// Pentium-3 (Feb. 1999)
#  define MMX
#endif

#include <immintrin.h>

#if defined(MMX)
/*!
  \namespace	mm
  \brief	Intel SIMD命令を利用するためのクラスおよび関数を納める名前空間
*/
namespace mm
{
/************************************************************************
*  type traits								*
************************************************************************/
template <class T>	struct type_traits;

template <>
struct type_traits<int8_t>
{
    typedef int8_t	signed_type;
    typedef int16_t	upper_type;
    typedef void	lower_type;
    enum
    {
	is_bool	  = false,
	is_signed = true,
    };
};
    
template <>
struct type_traits<u_int8_t>
{
    typedef int8_t	signed_type;
    typedef int16_t	upper_type;
    typedef void	lower_type;
    enum
    {
	is_bool	  = false,
	is_signed = false,
    };
};
    
template <>
struct type_traits<int16_t>
{
    typedef int16_t	signed_type;
    typedef int32_t	upper_type;
    typedef int8_t	lower_type;
    enum
    {
	is_bool	  = false,
	is_signed = true,
    };
};
    
template <>
struct type_traits<u_int16_t>
{
    typedef int16_t	signed_type;
    typedef int32_t	upper_type;
    typedef u_int8_t	lower_type;
    enum
    {
	is_bool	  = false,
	is_signed = false,
    };
};
    
template <>
struct type_traits<int32_t>
{
    typedef int32_t	signed_type;
    typedef int64_t	upper_type;
    typedef int16_t	lower_type;
    enum
    {
	is_bool	  = false,
	is_signed = true,
    };
};
    
template <>
struct type_traits<u_int32_t>
{
    typedef int32_t	signed_type;
    typedef int64_t	upper_type;
    typedef u_int16_t	lower_type;
    enum
    {
	is_bool	  = false,
	is_signed = false,
    };
};
    
template <>
struct type_traits<int64_t>
{
    typedef int64_t	signed_type;
    typedef void	upper_type;
    typedef int32_t	lower_type;
    enum
    {
	is_bool	  = false,
	is_signed = true,
    };
};
    
template <>
struct type_traits<u_int64_t>
{
    typedef int64_t	signed_type;
    typedef void	upper_type;
    typedef u_int32_t	lower_type;
    enum
    {
	is_bool	  = false,
	is_signed = false,
    };
};
    
/************************************************************************
*  SIMD vector types							*
************************************************************************/
#if defined(AVX2)
  typedef __m256i	ivec_t;		//!< 整数ベクトルのSIMD型
#elif defined(SSE2)  
  typedef __m128i	ivec_t;		//!< 整数ベクトルのSIMD型
#else
  typedef __m64		ivec_t;		//!< 整数ベクトルのSIMD型
#endif
    
#if defined(AVX)
  typedef __m256	fvec_t;		//!< floatベクトルのSIMD型
  typedef __m256d	dvec_t;		//!< doubleベクトルのSIMD型
#elif defined(SSE)
  typedef __m128	fvec_t;		//!< floatベクトルのSIMD型
#  if defined(SSE2)  
  typedef __m128d	dvec_t;		//!< doubleベクトルのSIMD型
#  endif
#endif

//! T型整数の成分を持つSIMDベクトルを表すクラス
template <class T>
class vec
{
  public:
    typedef T		value_type;	//!< 成分の型
    typedef ivec_t	base_type;	//!< ベースとなるSIMDデータ型
      
    enum	{value_size = sizeof(value_type),
		 size	    = sizeof(base_type)/sizeof(value_type),
		 lane_size  = (sizeof(base_type) > 16 ? 16/sizeof(value_type)
						      : size)};

    vec()						{}
    vec(value_type a)					;
    vec(value_type a1, value_type a0)			;
    vec(value_type a3, value_type a2,
	value_type a1, value_type a0)			;
    vec(value_type a7, value_type a6,
	value_type a5, value_type a4,
	value_type a3, value_type a2,
	value_type a1, value_type a0)			;
    vec(value_type a15, value_type a14,
	value_type a13, value_type a12,
	value_type a11, value_type a10,
	value_type a9,  value_type a8,
	value_type a7,  value_type a6,
	value_type a5,  value_type a4,
	value_type a3,  value_type a2,
	value_type a1,  value_type a0)			;
    vec(value_type a31, value_type a30,
	value_type a29, value_type a28,
	value_type a27, value_type a26,
	value_type a25, value_type a24,
	value_type a23, value_type a22,
	value_type a21, value_type a20,
	value_type a19, value_type a18,
	value_type a17, value_type a16,
	value_type a15, value_type a14,
	value_type a13, value_type a12,
	value_type a11, value_type a10,
	value_type a9,  value_type a8,
	value_type a7,  value_type a6,
	value_type a5,  value_type a4,
	value_type a3,  value_type a2,
	value_type a1,  value_type a0)			;
    
  // ベース型との間の型変換
    vec(base_type m)	:_base(m)			{}
			operator base_type()		{return _base;}

    vec<value_type>&	flip_sign()			;
    vec<value_type>&	operator +=(vec<value_type> x)	;
    vec<value_type>&	operator -=(vec<value_type> x)	;
    vec<value_type>&	operator *=(vec<value_type> x)	;
    vec<value_type>&	operator &=(vec<value_type> x)	;
    vec<value_type>&	operator |=(vec<value_type> x)	;
    vec<value_type>&	operator ^=(vec<value_type> x)	;
    vec<value_type>&	andnot(vec<value_type> x)	;

    int			operator [](int i)	const	;
    value_type&		operator [](int i)		;
    
    static size_t	floor(size_t n)	{return size*(n/size);}
    static size_t	ceil(size_t n)	{return (n == 0 ? 0 :
						 size*((n - 1)/size + 1));}

  private:
    base_type		_base;
};

template <class T> inline int
vec<T>::operator [](int i) const
{
    assert((0 <= i) && (i < size));
    return *((value_type*)&_base + i);
}
    
template <class T> inline typename vec<T>::value_type&
vec<T>::operator [](int i)
{
    assert((0 <= i) && (i < size));
    return *((value_type*)&_base + i);
}

typedef vec<int8_t>	Is8vec;		//!< 符号付き8bit整数ベクトル
typedef vec<u_int8_t>	Iu8vec;		//!< 符号なし8bit整数ベクトル
typedef vec<int16_t>	Is16vec;	//!< 符号付き16bit整数ベクトル
typedef vec<u_int16_t>	Iu16vec;	//!< 符号なし16bit整数ベクトル
typedef vec<int32_t>	Is32vec;	//!< 符号付き32bit整数ベクトル
typedef vec<u_int32_t>	Iu32vec;	//!< 符号なし32bit整数ベクトル
typedef vec<u_int64_t>	I64vec;		//!< 64bit整数ベクトル
    
#if defined(SSE)
//! float型の成分を持つSIMDベクトルを表すクラス
template <>
class vec<float>
{
  public:
    typedef float	value_type;	//!< 成分の型
    typedef fvec_t	base_type;	//!< ベースとなるSIMDデータ型
      
    enum	{value_size = sizeof(value_type),
		 size	    = sizeof(base_type)/sizeof(value_type)};

    vec()						{}
    vec(value_type a)					;
    vec(value_type a3, value_type a2,
	value_type a1, value_type a0)			;
    vec(value_type a7, value_type a6,
	value_type a5, value_type a4,
	value_type a3, value_type a2,
	value_type a1, value_type a0)			;

  // ベース型との間の型変換
    vec(base_type m)	:_base(m)			{}
			operator base_type()		{return _base;}

    vec<value_type>&	flip_sign()			;
    vec<value_type>&	operator +=(vec<value_type> x)	;
    vec<value_type>&	operator -=(vec<value_type> x)	;
    vec<value_type>&	operator *=(vec<value_type> x)	;
    vec<value_type>&	operator /=(vec<value_type> x)	;
    vec<value_type>&	operator &=(vec<value_type> x)	;
    vec<value_type>&	operator |=(vec<value_type> x)	;
    vec<value_type>&	operator ^=(vec<value_type> x)	;
    vec<value_type>&	andnot(vec<value_type> x)	;

    const value_type&	operator [](int i)	const	;
    value_type&		operator [](int i)		;
    
    static size_t	floor(size_t n)	{return size*(n/size);}
    static size_t	ceil(size_t n)	{return (n == 0 ? 0 :
						 size*((n - 1)/size + 1));}

  private:
    base_type		_base;
};

inline const typename vec<float>::value_type&
vec<float>::operator [](int i) const
{
    assert((0 <= i) && (i < size));
    return *((value_type*)&_base + i);
}
    
inline typename vec<float>::value_type&
vec<float>::operator [](int i)
{
    assert((0 <= i) && (i < size));
    return *((value_type*)&_base + i);
}

typedef vec<float>	F32vec;		//!< 32bit浮動小数点数ベクトル
#endif

#if defined(SSE2)
//! double型の成分を持つSIMDベクトルを表すクラス
template <>
class vec<double>
{
  public:
    typedef double	value_type;	//!< 成分の型
    typedef dvec_t	base_type;	//!< ベースとなるSIMDデータ型
      
    enum	{value_size = sizeof(value_type),
		 size	    = sizeof(base_type)/sizeof(value_type)};

    vec()						{}
    vec(value_type a)					;
    vec(value_type a1, value_type a0)			;
    vec(value_type a3, value_type a2,
	value_type a1, value_type a0)			;

  // ベース型との間の型変換
    vec(base_type m)	:_base(m)			{}
			operator base_type()		{return _base;}

    vec<value_type>&	flip_sign()			;
    vec<value_type>&	operator +=(vec<value_type> x)	;
    vec<value_type>&	operator -=(vec<value_type> x)	;
    vec<value_type>&	operator *=(vec<value_type> x)	;
    vec<value_type>&	operator /=(vec<value_type> x)	;
    vec<value_type>&	operator &=(vec<value_type> x)	;
    vec<value_type>&	operator |=(vec<value_type> x)	;
    vec<value_type>&	operator ^=(vec<value_type> x)	;
    vec<value_type>&	andnot(vec<value_type> x)	;

    const value_type&	operator [](int i)	const	;
    value_type&		operator [](int i)		;
    
    static size_t	floor(size_t n)	{return size*(n/size);}
    static size_t	ceil(size_t n)	{return (n == 0 ? 0 :
						 size*((n - 1)/size + 1));}

  private:
    base_type		_base;
};

inline const typename vec<double>::value_type&
vec<double>::operator [](int i) const
{
    assert((0 <= i) && (i < size));
    return *((value_type*)&_base + i);
}
    
inline typename vec<double>::value_type&
vec<double>::operator [](int i)
{
    assert((0 <= i) && (i < size));
    return *((value_type*)&_base + i);
}

typedef vec<double>	F64vec;		//!< 64bit浮動小数点数ベクトル
#endif

//! SIMDベクトルの内容をストリームに出力する．
/*!
  \param out	出力ストリーム
  \param vec	SIMDベクトル
  \return	outで指定した出力ストリーム
*/
template <class T> std::ostream&
operator <<(std::ostream& out, const vec<T>& x)
{
    for (size_t i = 0; i < vec<T>::size; ++i)
	out << ' ' << x[i];

    return out;
}

/************************************************************************
*  Macros for constructing mnemonics of intrinsics			*
************************************************************************/
#define MM_PREFIX(type)		MM_PREFIX_##type
#define MM_SUFFIX(type)		MM_SUFFIX_##type
#define MM_SIGNED(type)		MM_SIGNED_##type
#define MM_BASE(type)		MM_BASE_##type

#if defined(AVX2)
#  define MM_PREFIX_int8_t	_mm256_
#  define MM_PREFIX_u_int8_t	_mm256_
#  define MM_PREFIX_int16_t	_mm256_
#  define MM_PREFIX_u_int16_t	_mm256_
#  define MM_PREFIX_int32_t	_mm256_
#  define MM_PREFIX_u_int32_t	_mm256_
#  define MM_PREFIX_u_int64_t	_mm256_
#  define MM_PREFIX_ivec_t	_mm256_
#else
#  define MM_PREFIX_int8_t	_mm_
#  define MM_PREFIX_u_int8_t	_mm_
#  define MM_PREFIX_int16_t	_mm_
#  define MM_PREFIX_u_int16_t	_mm_
#  define MM_PREFIX_int32_t	_mm_
#  define MM_PREFIX_u_int32_t	_mm_
#  define MM_PREFIX_u_int64_t	_mm_
#  define MM_PREFIX_ivec_t	_mm_
#endif

#if defined(SSE2)
#  define MM_SUFFIX_int8_t	epi8
#  define MM_SUFFIX_u_int8_t	epu8
#  define MM_SUFFIX_int16_t	epi16
#  define MM_SUFFIX_u_int16_t	epu16
#  define MM_SUFFIX_int32_t	epi32
#  define MM_SUFFIX_u_int32_t	epu32
#  define MM_SUFFIX_u_int64_t	epi64
#  if defined(AVX2)
#    define MM_SUFFIX_ivec_t	si256
#  else
#    define MM_SUFFIX_ivec_t	si128
#  endif
#else
#  define MM_SUFFIX_int8_t	pi8
#  define MM_SUFFIX_u_int8_t	pu8
#  define MM_SUFFIX_int16_t	pi16
#  define MM_SUFFIX_u_int16_t	pu16
#  define MM_SUFFIX_int32_t	pi32
#  define MM_SUFFIX_u_int32_t	pu32
#  define MM_SUFFIX_u_int64_t	si64
#  define MM_SUFFIX_ivec_t	si64
#endif
#define MM_SUFFIX_void

#define MM_SIGNED_int8_t	MM_SUFFIX_int8_t
#define MM_SIGNED_u_int8_t	MM_SUFFIX_int8_t
#define MM_SIGNED_int16_t	MM_SUFFIX_int16_t
#define MM_SIGNED_u_int16_t	MM_SUFFIX_int16_t
#define MM_SIGNED_int32_t	MM_SUFFIX_int32_t
#define MM_SIGNED_u_int32_t	MM_SUFFIX_int32_t
#define MM_SIGNED_u_int64_t	MM_SUFFIX_u_int64_t
    
#define MM_BASE_int8_t		MM_SUFFIX_ivec_t
#define MM_BASE_u_int8_t	MM_SUFFIX_ivec_t
#define MM_BASE_int16_t		MM_SUFFIX_ivec_t
#define MM_BASE_u_int16_t	MM_SUFFIX_ivec_t
#define MM_BASE_int32_t		MM_SUFFIX_ivec_t
#define MM_BASE_u_int32_t	MM_SUFFIX_ivec_t
#define MM_BASE_u_int64_t	MM_SUFFIX_ivec_t
#define MM_BASE_ivec_t		MM_SUFFIX_ivec_t

#if defined(AVX)
#  define MM_PREFIX_float	_mm256_
#  define MM_PREFIX_fvec_t	_mm256_
#  define MM_PREFIX_double	_mm256_
#  define MM_PREFIX_dvec_t	_mm256_
#elif defined(SSE)
#  define MM_PREFIX_float	_mm_
#  define MM_PREFIX_fvec_t	_mm_
#  if defined(SSE2)
#    define MM_PREFIX_double	_mm_
#    define MM_PREFIX_dvec_t	_mm_
#  endif
#endif

#if defined(SSE)
#  define MM_SUFFIX_float	ps
#  define MM_SUFFIX_fvec_t	ps
#  define MM_SIGNED_float	ps
#  define MM_BASE_float		ps
#  define MM_BASE_fvec_t	ps
#endif    
#if defined(SSE2)
#  define MM_SUFFIX_double	pd
#  define MM_SUFFIX_dvec_t	pd
#  define MM_SIGNED_double	pd
#  define MM_BASE_double	pd
#  define MM_BASE_dvec_t	pd
#endif    

#define MM_CAT(op, prefix, from, suffix)	prefix##op##from##_##suffix
#define MM_MNEMONIC(op, prefix, from, suffix)	MM_CAT(op, prefix, from, suffix)

#define MM_TMPL_FUNC(signature, op, args, from, to, suffix)		\
    inline signature							\
    {									\
	return MM_MNEMONIC(op, MM_PREFIX(to),				\
			   MM_SUFFIX(from), suffix(to))args;		\
    }

#define MM_FUNC(signature, op, args, from, to, suffix)			\
    template <> MM_TMPL_FUNC(signature, op, args, from, to, suffix)

#define MM_FUNC_2(func, op, type)					\
    MM_FUNC(vec<type> func(vec<type> x, vec<type> y),			\
	    op, (x, y), void, type, MM_SIGNED)

#define MM_BASE_FUNC_2(func, op, type)					\
    MM_FUNC(vec<type> func(vec<type> x, vec<type> y),			\
	    op, (x, y), void, type, MM_BASE)

#define MM_NUMERIC_FUNC_1(func, op, type)				\
    MM_FUNC(vec<type> func(vec<type> x),				\
	    op, (x), void, type, MM_SUFFIX)

#define MM_NUMERIC_FUNC_2(func, op, type)				\
    MM_FUNC(vec<type> func(vec<type> x, vec<type> y),			\
	    op, (x, y), void, type, MM_SUFFIX)

#define MM_NUMERIC_FUNC_2R(func, op, type)				\
    MM_FUNC(vec<type> func(vec<type> x, vec<type> y),			\
	    op, (y, x), void, type, MM_SUFFIX)

/************************************************************************
*  Constructors of vec<T>						*
************************************************************************/
#define MM_CONSTRUCTOR_1(type)						\
    inline								\
    vec<type>::vec(value_type a)					\
	:_base(MM_MNEMONIC(set1, MM_PREFIX(type), , MM_SIGNED(type))	\
	       (a))							\
    {									\
    }
#define MM_CONSTRUCTOR_2(type)						\
    inline								\
    vec<type>::vec(value_type a1, value_type a0)			\
	:_base(MM_MNEMONIC(set, MM_PREFIX(type), , MM_SIGNED(type))	\
	       (a1, a0))						\
    {									\
    }
#define MM_CONSTRUCTOR_4(type)						\
    inline								\
    vec<type>::vec(value_type a3, value_type a2,			\
		   value_type a1, value_type a0)			\
	:_base(MM_MNEMONIC(set,  MM_PREFIX(type), , MM_SIGNED(type))	\
	       (a3, a2, a1, a0))					\
    {									\
    }
#define MM_CONSTRUCTOR_8(type)						\
    inline								\
    vec<type>::vec(value_type a7, value_type a6,			\
		   value_type a5, value_type a4,			\
		   value_type a3, value_type a2,			\
		   value_type a1, value_type a0)			\
	:_base(MM_MNEMONIC(set,  MM_PREFIX(type), , MM_SIGNED(type))	\
	       (a7, a6, a5, a4,	a3, a2, a1, a0))			\
    {									\
    }
#define MM_CONSTRUCTOR_16(type)						\
    inline								\
    vec<type>::vec(value_type a15, value_type a14,			\
		   value_type a13, value_type a12,			\
		   value_type a11, value_type a10,			\
		   value_type a9,  value_type a8,			\
		   value_type a7,  value_type a6,			\
		   value_type a5,  value_type a4,			\
		   value_type a3,  value_type a2,			\
		   value_type a1,  value_type a0)			\
	:_base(MM_MNEMONIC(set,  MM_PREFIX(type), , MM_SIGNED(type))	\
	       (a15, a14, a13, a12, a11, a10, a9, a8,			\
		a7,  a6,  a5,  a4,  a3,  a2,  a1, a0))			\
    {									\
    }
#define MM_CONSTRUCTOR_32(type)						\
    inline								\
    vec<type>::vec(value_type a31, value_type a30,			\
		   value_type a29, value_type a28,			\
		   value_type a27, value_type a26,			\
		   value_type a25, value_type a24,			\
		   value_type a23, value_type a22,			\
		   value_type a21, value_type a20,			\
		   value_type a19, value_type a18,			\
		   value_type a17, value_type a16,			\
		   value_type a15, value_type a14,			\
		   value_type a13, value_type a12,			\
		   value_type a11, value_type a10,			\
		   value_type a9,  value_type a8,			\
		   value_type a7,  value_type a6,			\
		   value_type a5,  value_type a4,			\
		   value_type a3,  value_type a2,			\
		   value_type a1,  value_type a0)			\
	:_base(MM_MNEMONIC(set,  MM_PREFIX(type), , MM_SIGNED(type))	\
	       (a31, a30, a29, a28, a27, a26, a25, a24,			\
		a23, a22, a21, a20, a19, a18, a17, a16,			\
		a15, a14, a13, a12, a11, a10, a9,  a8,			\
		a7,  a6,  a5,  a4,  a3,  a2,  a1,  a0))			\
    {									\
    }

MM_CONSTRUCTOR_1(int8_t)
MM_CONSTRUCTOR_1(int16_t)
MM_CONSTRUCTOR_1(int32_t)
MM_CONSTRUCTOR_1(u_int8_t)
MM_CONSTRUCTOR_1(u_int16_t)
MM_CONSTRUCTOR_1(u_int32_t)

#if defined(AVX2)
  MM_CONSTRUCTOR_32(int8_t)	
  MM_CONSTRUCTOR_32(u_int8_t)	
  MM_CONSTRUCTOR_16(int16_t)	
  MM_CONSTRUCTOR_16(u_int16_t)	
  MM_CONSTRUCTOR_8(int32_t)
  MM_CONSTRUCTOR_8(u_int32_t)
#elif defined(SSE2)
  MM_CONSTRUCTOR_16(int8_t)
  MM_CONSTRUCTOR_8(int16_t)
  MM_CONSTRUCTOR_4(int32_t)
  MM_CONSTRUCTOR_16(u_int8_t)
  MM_CONSTRUCTOR_8(u_int16_t)
  MM_CONSTRUCTOR_4(u_int32_t)
#else
  MM_CONSTRUCTOR_8(int8_t)
  MM_CONSTRUCTOR_4(int16_t)
  MM_CONSTRUCTOR_2(int32_t)
  MM_CONSTRUCTOR_8(u_int8_t)
  MM_CONSTRUCTOR_4(u_int16_t)
  MM_CONSTRUCTOR_2(u_int32_t)
#endif

#if defined(SSE)
  MM_CONSTRUCTOR_1(float)
#  if defined(AVX)
  MM_CONSTRUCTOR_8(float)
#  else
  MM_CONSTRUCTOR_4(float)
#  endif
#endif

#if defined(SSE2)
  MM_CONSTRUCTOR_1(double)
#  if defined(AVX) 
  MM_CONSTRUCTOR_4(double)
#  else
  MM_CONSTRUCTOR_2(double)
#  endif
#endif

#undef MM_CONSTRUCTOR_1
#undef MM_CONSTRUCTOR_2
#undef MM_CONSTRUCTOR_4
#undef MM_CONSTRUCTOR_8
#undef MM_CONSTRUCTOR_16
#undef MM_CONSTRUCTOR_32

/************************************************************************
*  Load/Store								*
************************************************************************/
//! 16byteにalignされたメモリからベクトルをロードする．
/*!
  \param p	16byteにalignされたロード元のメモリアドレス
  \return	ロードされたベクトル
*/
template <class T> static vec<T>	load(const T* p)		;

//! メモリからベクトルをロードする．
/*!
  \param p	ロード元のメモリアドレス
  \return	ロードされたベクトル
*/
template <class T> static vec<T>	loadu(const T* p)		;

//! 16byteにalignされたメモリにベクトルをストアする．
/*!
  \param p	16byteにalignされたストア先のメモリアドレス
  \param x	ストアされるベクトル
*/
template <class T> static void		store(T* p, vec<T> x)		;

//! メモリにベクトルをストアする．
/*!
  \param p	ストア先のメモリアドレス
  \param x	ストアされるベクトル
*/
template <class T> static void		storeu(T* p, vec<T> x)		;

#if defined(SSE2)
#  define MM_LOAD_STORE(type)						\
    MM_FUNC(vec<type> load(const type* p), load,			\
	    ((const vec<type>::base_type*)p), void, type, MM_BASE)	\
    MM_FUNC(vec<type> loadu(const type* p), loadu,			\
	    ((const vec<type>::base_type*)p), void, type, MM_BASE)	\
    MM_FUNC(void store(type* p, vec<type> x), store,			\
	    ((vec<type>::base_type*)p, x), void, type, MM_BASE)		\
    MM_FUNC(void storeu(type* p, vec<type> x), storeu,			\
	    ((vec<type>::base_type*)p, x), void, type, MM_BASE)

  MM_LOAD_STORE(int8_t)
  MM_LOAD_STORE(int16_t)
  MM_LOAD_STORE(int32_t)
  MM_LOAD_STORE(u_int8_t)
  MM_LOAD_STORE(u_int16_t)
  MM_LOAD_STORE(u_int32_t)
  MM_LOAD_STORE(u_int64_t)

#  undef MM_LOAD_STORE  
#else
  template <class T> inline vec<T>
  load(const T* p)
  {
      return *((const typename vec<T>::base_type*)p);
  }
  template <class T> inline vec<T>
  loadu(const T* p)
  {
      return load(p);
  }
  template <class T> inline void
  store(T* p, vec<T> x)
  {
      *((typename vec<T>::base_type*)p) = x;
  }
  template <class T> inline void
  storeu(T* p, vec<T> x)
  {
      store(p, x);
  }
#endif

#if defined(SSE)
#  define MM_LOAD_STORE(type)						\
    MM_FUNC(vec<type> load(const type* p), load,			\
	    ((const type*)p), void, type, MM_BASE)			\
    MM_FUNC(vec<type> loadu(const type* p), loadu,			\
	    ((const type*)p), void, type, MM_BASE)			\
    MM_FUNC(void store(type* p, vec<type> x), store,			\
	    ((type*)p, x), void, type, MM_BASE)				\
    MM_FUNC(void storeu(type* p, vec<type> x), storeu,			\
	    ((type*)p, x), void, type, MM_BASE)

  MM_LOAD_STORE(float)
#  if defined(SSE2)
  MM_LOAD_STORE(double)
#  endif
#  undef MM_LOAD_STORE
#endif
  
/************************************************************************
*  Zero-vector generators						*
************************************************************************/
//! 全成分が0であるベクトルを生成する．
template <class T> static vec<T>	zero()				;

#define MM_ZERO(type)							\
    MM_FUNC(vec<type> zero<type>(), setzero, (), void, type, MM_BASE)

MM_ZERO(int8_t)
MM_ZERO(int16_t)
MM_ZERO(int32_t)
MM_ZERO(u_int8_t)
MM_ZERO(u_int16_t)
MM_ZERO(u_int32_t)
MM_ZERO(u_int64_t)
    
#if defined(SSE)
  MM_ZERO(float)
#endif
#if defined(SSE2)
  MM_ZERO(double)
#endif

#undef MM_ZERO

/************************************************************************
*  Cast operators							*
************************************************************************/
template <class S, class T> static S	cast_base(T x)			;

//! T型の成分を持つベクトルからS型の成分を持つベクトルへのキャストを行なう．
template <class S, class T> static inline vec<S>
cast(vec<T> x)
{
    return
	cast_base<typename vec<S>::base_type>(typename vec<T>::base_type(x));
}

/*
 *  cast_base() の実装
 */
// 整数 <-> 整数
template <> inline ivec_t
cast_base(ivec_t x)
{
    return x;
}

// 整数 <-> float, double
#if !defined(AVX2) && defined(AVX)
  template <> fvec_t
  cast_base<fvec_t>(ivec_t x)
  {
      return _mm256_castsi256_ps(_mm256_castsi128_si256(x));
  }

  template <> ivec_t
  cast_base<ivec_t>(fvec_t x)
  {
      return _mm256_castsi256_si128(_mm256_castps_si256(x));
  }

  template <> dvec_t
  cast_base<dvec_t>(ivec_t x)
  {
      return _mm256_castsi256_pd(_mm256_castsi128_si256(x));
  }

  template <> ivec_t
  cast_base<ivec_t>(dvec_t x)
  {
      return _mm256_castsi256_si128(_mm256_castpd_si256(x));
  }
#elif defined(SSE2)
  MM_FUNC(fvec_t cast_base<fvec_t>(ivec_t x),
	  cast, (x), ivec_t, fvec_t, MM_BASE)
  MM_FUNC(ivec_t cast_base<ivec_t>(fvec_t x),
	  cast, (x), fvec_t, ivec_t, MM_BASE)
  MM_FUNC(dvec_t cast_base<dvec_t>(ivec_t x),
	  cast, (x), ivec_t, dvec_t, MM_BASE)
  MM_FUNC(ivec_t cast_base<ivec_t>(dvec_t x),
	  cast, (x), dvec_t, ivec_t, MM_BASE)
#endif

// float <-> double
#if defined(SSE2)
  MM_FUNC(dvec_t cast_base<dvec_t>(fvec_t x),
	  cast, (x), fvec_t, dvec_t, MM_BASE)
  MM_FUNC(fvec_t cast_base<fvec_t>(dvec_t x),
	  cast, (x), dvec_t, fvec_t, MM_BASE)
#endif
  
/************************************************************************
*  Shuffle operators							*
************************************************************************/
//! 8つの成分を持つ整数ベクトルの下位4成分をシャッフルする．
/*!
  上位4成分は変化しない．
  \param I0	最下位に来る成分のindex (0 <= I0 < 4)
  \param I1	下から2番目に来る成分のindex (0 <= I1 < 4)
  \param I2	下から3番目に来る成分のindex (0 <= I2 < 4)
  \param I3	下から4番目に来る成分のindex (0 <= I3 < 4)
  \param x	シャッフルされるベクトル
  \return	シャッフルされたベクトル
*/
template <u_int I3, u_int I2, u_int I1, u_int I0, class T> static vec<T>
shuffle_low(vec<T> x)							;

//! 8つの成分を持つ整数ベクトルの上位4成分をシャッフルする．
/*!
  下位4成分は変化しない．
  \param I0	下から5番目に来る成分のindex (0 <= I0 < 4)
  \param I1	下から6番目に来る成分のindex (0 <= I1 < 4)
  \param I2	下から7番目に来る成分のindex (0 <= I2 < 4)
  \param I3	最上位に来る成分のindex (0 <= I3 < 4)
  \param x	シャッフルされるベクトル
  \return	シャッフルされたベクトル
*/
template <u_int I3, u_int I2, u_int I1, u_int I0, class T> static vec<T>
shuffle_high(vec<T> x)							;

//! 4つの成分を持つ整数ベクトルの成分をシャッフルする．
/*!
  \param I0	最下位に来る成分のindex (0 <= I0 < 4)
  \param I1	下から2番目に来る成分のindex (0 <= I1 < 4)
  \param I2	下から3番目に来る成分のindex (0 <= I2 < 4)
  \param I3	最上位に来る成分のindex (0 <= I3 < 4)
  \param x	シャッフルされるベクトル
  \return	シャッフルされたベクトル
*/
template <u_int I3, u_int I2, u_int I1, u_int I0, class T> static vec<T>
shuffle(vec<T> x)							;

#define MM_SHUFFLE_LOW_HIGH_I4(type)					\
    template <u_int I3, u_int I2, u_int I1, u_int I0>			\
    MM_TMPL_FUNC(vec<type> shuffle_low(vec<type> x),			\
		 shufflelo, (x, _MM_SHUFFLE(I3, I2, I1, I0)),		\
		 void, type, MM_SIGNED)					\
    template <u_int I3, u_int I2, u_int I1, u_int I0>			\
    MM_TMPL_FUNC(vec<type> shuffle_high(vec<type> x),			\
		 shufflehi, (x, _MM_SHUFFLE(I3, I2, I1, I0)),		\
		 void, type, MM_SIGNED)
#define MM_SHUFFLE_I4(type)						\
    template <u_int I3, u_int I2, u_int I1, u_int I0>			\
    MM_TMPL_FUNC(vec<type> shuffle(vec<type> x),			\
		 shuffle, (x, _MM_SHUFFLE(I3, I2, I1, I0)),		\
		 void, type, MM_SIGNED)

#if defined(SSE2)
  MM_SHUFFLE_I4(int32_t)
  MM_SHUFFLE_I4(u_int32_t)
  MM_SHUFFLE_LOW_HIGH_I4(int16_t)
  MM_SHUFFLE_LOW_HIGH_I4(u_int16_t)
#elif defined(SSE)
  MM_SHUFFLE_I4(int16_t)
  MM_SHUFFLE_I4(u_int16_t)
#endif
  
#undef MM_SHUFFLE_LOW_HIGH_I4
#undef MM_SHUFFLE_I4

//! 4つの成分を持つ2つの浮動小数点数ベクトルの成分をシャッフルする．
/*!
  下位2成分はxから，上位2成分はyからそれぞれ選択される．
  \param Xl	最下位に来るベクトルxの成分のindex (0 <= Xl < 4)
  \param Xh	下から2番目に来るベクトルxの成分のindex (0 <= Xh < 4)
  \param Yl	下から3番目に来るベクトルyの成分のindex (0 <= Yl < 4)
  \param Yh	最上位に来るベクトルyの成分のindex (0 <= Yh < 4)
  \param x	シャッフルされるベクトル
  \param y	シャッフルされるベクトル
  \return	シャッフルされたベクトル
*/
template <u_int Yh, u_int Yl, u_int Xh, u_int Xl, class T> static vec<T>
shuffle(vec<T> x, vec<T> y)						;

//! 2つの成分を持つ2つの浮動小数点数ベクトルの成分をシャッフルする．
/*!
  下位成分はxから，上位成分はyからそれぞれ選択される．
  \param X	下位に来るベクトルxの成分のindex (0 <= I0 < 2)
  \param Y	上位に来るベクトルyの成分のindex (0 <= I3 < 2)
  \param x	シャッフルされるベクトル
  \param y	シャッフルされるベクトル
  \return	シャッフルされたベクトル
*/
template <u_int Y, u_int X, class T> static vec<T>
shuffle(vec<T> x, vec<T> y)						;

#define _MM_SHUFFLE4(i3, i2, i1, i0)					\
    (((i3) << 3) | ((i2) << 2) | ((i1) << 1) | (i0))
#define MM_SHUFFLE_F4(type)						\
    template <u_int Yh, u_int Yl, u_int Xh, u_int Xl>			\
    MM_TMPL_FUNC(vec<type> shuffle(vec<type> x, vec<type> y),		\
		 shuffle, (x, y, _MM_SHUFFLE(Yh, Yl, Xh, Xl)),		\
		 void, type, MM_SUFFIX)
#define MM_SHUFFLE_D4(type)						\
    template <u_int Yh, u_int Yl, u_int Xh, u_int Xl>			\
    MM_TMPL_FUNC(vec<type> shuffle(vec<type> x, vec<type> y),		\
		 shuffle, (x, y, _MM_SHUFFLE4(Yh, Yl, Xh, Xl)),		\
		 void, type, MM_SUFFIX)
#define MM_SHUFFLE_D2(type)						\
    template <u_int Y, u_int X>						\
    MM_TMPL_FUNC(vec<type> shuffle(vec<type> x, vec<type> y),		\
		 shuffle, (x, y, _MM_SHUFFLE2(Y, X)),			\
		 void, type, MM_SUFFIX)

#if defined(SSE)
  MM_SHUFFLE_F4(float)
#  if defined(AVX)
  MM_SHUFFLE_D4(double)
#  elif defined(SSE2)
  MM_SHUFFLE_D2(double)
#  endif
#endif

#undef MM_SHUFFLE_D2
#undef MM_SHUFFLE_F4

/************************************************************************
*  全成分にN番目の要素をセット						*
************************************************************************/
//! 与えられたベクトルの指定された成分を全成分にセットしたベクトルを生成する．
/*!
  与えられたベクトルの成分数は2または4でなければならない．
  \param N	セットするxのベクトルの成分を指定するindex (0 <= N < 4)
  \param x	2つ，または4つの成分を持つベクトル
  \return	生成されたベクトル
*/
template <u_int N, class T> static inline vec<T>
set1(vec<T> x)
{
      return shuffle<N, N, N, N>(x);
}

#if defined(SSE)
  template <u_int N> static inline F32vec
  set1(F32vec x)		{return shuffle<N, N, N, N>(x, x);}
#  if defined(AVX)
  template <u_int N> static inline F64vec
  set1(F64vec x)		{return shuffle<N, N, N, N>(x, x);}
#  elif defined(SSE2)
  template <u_int N> static inline F64vec
  set1(F64vec x)		{return shuffle<N, N>(x, x);}
#  endif
#endif

/************************************************************************
*  Unpack operators							*
************************************************************************/
//! 2つのベクトルの下位半分の成分を交互に混合する．
/*!
  \param x	その成分を偶数番目に配置するベクトル
  \param y	その成分を奇数番目に配置するベクトル
  \return	生成されたベクトル
*/
template <class T> static vec<T>	unpack_low(vec<T> x, vec<T> y)	;

//! 2つのベクトルの上位半分の成分を交互に混合する．
/*!
  \param x	その成分を偶数番目に配置するベクトル
  \param y	その成分を奇数番目に配置するベクトル
  \return	生成されたベクトル
*/
template <class T> static vec<T>	unpack_high(vec<T> x, vec<T> y)	;

#define MM_UNPACK_LOW_HIGH(type)					\
    MM_FUNC_2(unpack_low,  unpacklo, type)				\
    MM_FUNC_2(unpack_high, unpackhi, type)

MM_UNPACK_LOW_HIGH(int8_t)
MM_UNPACK_LOW_HIGH(int16_t)
MM_UNPACK_LOW_HIGH(int32_t)
MM_UNPACK_LOW_HIGH(u_int8_t)
MM_UNPACK_LOW_HIGH(u_int16_t)
MM_UNPACK_LOW_HIGH(u_int32_t)
#if defined(SSE)
  MM_UNPACK_LOW_HIGH(float)
#  if defined(SSE2)
  MM_UNPACK_LOW_HIGH(u_int64_t)
  MM_UNPACK_LOW_HIGH(double)
#  endif
#endif

#undef MM_UNPACK_LOW_HIGH

/************************************************************************
*  N-tuple generators							*
************************************************************************/
// 複製数：N = 2, 4, 8, 16,...;
// 全体をN個の部分に分けたときの複製区間：0 <= I < N
template <u_int N, u_int I, class T> static vec<T>	n_tuple(vec<T> x);

template <u_int I, class T> static inline vec<T>
dup(vec<T> x)
{
    return n_tuple<2, I>(x);
}

template <u_int I, class T> static inline vec<T>
quadup(vec<T> x)
{
    return n_tuple<4, I>(x);
}
    
template <u_int I, class T> static inline vec<T>
octup(vec<T> x)
{
    return n_tuple<8, I>(x);
}
    
#define MM_N_TUPLE(type)						\
    template <> inline vec<type>					\
    n_tuple<2, 0>(vec<type> x)		{return unpack_low(x, x);}	\
    template <> inline vec<type>					\
    n_tuple<2, 1>(vec<type> x)		{return unpack_high(x, x);}

template <u_int N, u_int I, class T> inline vec<T>
n_tuple(vec<T> x)
{
    return n_tuple<2, (I&0x1)>(n_tuple<(N>>1), (I>>1)>(x));
}

MM_N_TUPLE(int8_t)
MM_N_TUPLE(int16_t)
MM_N_TUPLE(int32_t)
MM_N_TUPLE(u_int8_t)
MM_N_TUPLE(u_int16_t)
MM_N_TUPLE(u_int32_t)
#if defined(SSE)
  MM_N_TUPLE(float)
#  if defined(SSE2)
  MM_N_TUPLE(u_int64_t)
  MM_N_TUPLE(double)
#  endif
#endif

#undef MM_N_TUPLE
    
/************************************************************************
*  Extracting elements							*
************************************************************************/
//! ベクトルから指定された成分を取り出す．
/*!
  \param I	取り出す成分を指定するindex
  \param x	ベクトル
  \return	取り出された成分
*/
template <u_int I, class T> static T	extract(vec<T> x)		;

#if defined(AVX2)
#  define MM_EXTRACT(type, base)					\
    template <u_int I> inline int					\
    extract(vec<type> x)						\
    {									\
	return MM_MNEMONIC(extract, _mm_, , MM_SIGNED(type))(		\
			       _mm256_extractf128_si256(		\
				   x,					\
				   (I < vec<type>::lane_size ? 0 : 1)),	\
			       I & (vec<type>::lane_size - 1));		\
    }
#else
#  define MM_EXTRACT(type)						\
    template <u_int I>							\
    MM_TMPL_FUNC(int extract(vec<type> x),				\
		 extract, (x, I), void, type, MM_SIGNED)
#endif

#if defined(SSE)
  MM_EXTRACT(int16_t)
  MM_EXTRACT(u_int16_t)
#  if defined(SSE4)
  MM_EXTRACT(int8_t)
  MM_EXTRACT(u_int8_t)
  MM_EXTRACT(int32_t)
  MM_EXTRACT(u_int32_t)
#  else
#  endif
#endif
  
#undef MM_EXTRACT

/************************************************************************
*  Elementwise shift operators						*
************************************************************************/
//! ベクトルの要素を左シフトする．
/*!
  シフト後の下位には0が入る．
  \param N	シフト数(成分単位)
  \param x	シフトされるベクトル
  \return	シフトされたベクトル
*/
template <u_int N, class T> static vec<T>	shift_l(vec<T> x)	;

//! ベクトルの要素を右シフトする．
/*!
  シフト後の上位には0が入る．
  \param N	シフト数(成分単位)
  \param x	シフトされるベクトル
  \return	シフトされたベクトル
*/
template <u_int N, class T> static vec<T>	shift_r(vec<T> x)	;

// 整数ベクトルの要素シフト（実装上の注意：MMXでは64bit整数のシフトは
// bit単位だが，SSE2以上の128bit整数ではbyte単位である．また，AVX2では
// 2つの128bit整数のシフトとなる．）
#if defined(AVX2)
#  define MM_ELM_SHIFTS_I(type)						\
    template <> inline vec<type>					\
    shift_l<0>(vec<type> x)				{return x;}	\
    template <> inline vec<type>					\
    shift_r<0>(vec<type> x)				{return x;}	\
    template <u_int N> inline vec<type>					\
    shift_l(vec<type> x)						\
    {									\
	if (N < vec<type>::lane_size)					\
	    return _mm256_alignr_epi8(					\
		     x, _mm256_permute2f128_si256(x, x, 0x08),		\
		     (vec<type>::lane_size - N)*vec<type>::value_size);	\
	else								\
	    return _mm256_alignr_epi8(					\
		     _mm256_permute2f128_si256(x, x, 0x08),		\
		     _mm256_setzero_si256(),				\
		     (vec<type>::size - N)*vec<type>::value_size);	\
    }									\
    template <u_int N> inline vec<type>					\
    shift_r(vec<type> x)						\
    {									\
	return _mm256_alignr_epi8(					\
		   _mm256_permute2f128_si256(x, x, 0x81), x,		\
		   N*vec<type>::value_size);				\
    }
#elif defined(SSE2)
#  define MM_ELM_SHIFTS_I(type)						\
    template <> inline vec<type>					\
    shift_l<0>(vec<type> x)				{return x;}	\
    template <> inline vec<type>					\
    shift_r<0>(vec<type> x)				{return x;}	\
    template <u_int N>							\
    MM_TMPL_FUNC(vec<type> shift_l(vec<type> x), slli,			\
		 (x, N*vec<type>::value_size), void, type, MM_BASE)	\
    template <u_int N>							\
    MM_TMPL_FUNC(vec<type> shift_r(vec<type> x), srli,			\
		 (x, N*vec<type>::value_size), void, type, MM_BASE)
#else
#  define MM_ELM_SHIFTS_I(type)						\
    template <> inline vec<type>					\
    shift_l<0>(vec<type> x)				{return x;}	\
    template <> inline vec<type>					\
    shift_r<0>(vec<type> x)				{return x;}	\
    template <u_int N>							\
    MM_TMPL_FUNC(vec<type> shift_l(vec<type> x), slli,			\
		 (x, 8*N*vec<type>::value_size), void, type, MM_BASE)	\
    template <u_int N>							\
    MM_TMPL_FUNC(vec<type> shift_r(vec<type> x), srli,			\
		 (x, 8*N*vec<type>::value_size), void, type, MM_BASE)
#endif

MM_ELM_SHIFTS_I(int8_t)
MM_ELM_SHIFTS_I(int16_t)
MM_ELM_SHIFTS_I(int32_t)
MM_ELM_SHIFTS_I(u_int8_t)
MM_ELM_SHIFTS_I(u_int16_t)
MM_ELM_SHIFTS_I(u_int32_t)
MM_ELM_SHIFTS_I(u_int64_t)

#undef MM_ELM_SHIFTS_I

// 浮動小数点数ベクトルの要素シフト
// （整数ベクトルと同一サイズの場合のみ定義できる）
#if defined(AVX2) || (!defined(AVX) && defined(SSE2))
  template <u_int N> inline vec<float>
  shift_l(vec<float> x)
  {
      return cast<float>(shift_l<N>(cast<u_int32_t>(x)));
  }

  template <u_int N> inline vec<float>
  shift_r(vec<float> x)
  {
      return cast<float>(shift_r<N>(cast<u_int32_t>(x)));
  }

  template <u_int N> inline vec<double>
  shift_l(vec<double> x)
  {
      return cast<double>(shift_l<N>(cast<u_int64_t>(x)));
  }

  template <u_int N> inline vec<double>
  shift_r(vec<double> x)
  {
      return cast<double>(shift_r<N>(cast<u_int64_t>(x)));
  }
#endif

/************************************************************************
*  Element wise shift to left/right-most				*
************************************************************************/
//! 左端の要素が右端に来るまで右シフトする．
/*!
  シフト後の上位には0が入る．
  \param x	シフトされるベクトル
  \return	シフトされたベクトル
*/
template <class T> static inline vec<T>
shift_lmost_to_rmost(vec<T> x)
{
    return shift_r<vec<T>::size-1>(x);
}

//! 右端の要素が左端に来るまで左シフトする．
/*!
  シフト後の下位には0が入る．
  \param x	シフトされるベクトル
  \return	シフトされたベクトル
*/
template <class T> static inline vec<T>
shift_rmost_to_lmost(vec<T> x)
{
    return shift_l<vec<T>::size-1>(x);
}

//! 与えられた値を右端の成分にセットし残りを0としたベクトルを生成する．
/*!
  \param x	セットされる値
  \return	xを右端成分とするベクトル
*/
template <class T> static inline vec<T>
set_rmost(typename vec<T>::value_type x)
{
    return shift_lmost_to_rmost(vec<T>(x));
}

/************************************************************************
*  Replacing rightmost element of x with that of y			*
************************************************************************/
#if defined(AVX)
  static inline F32vec
  replace_rmost(F32vec x, F32vec y)	{return _mm256_blend_ps(x, y, 0x01);}
  static inline F64vec
  replace_rmost(F64vec x, F64vec y)	{return _mm256_blend_pd(x, y, 0x01);}
#elif defined(SSE)
  static inline F32vec
  replace_rmost(F32vec x, F32vec y)	{return _mm_move_ss(x, y);}
#  if defined(SSE2)
    static inline F64vec
    replace_rmost(F64vec x, F64vec y)	{return _mm_move_sd(x, y);}
#  endif
#endif
    
/************************************************************************
*  Rotation and reverse operators					*
************************************************************************/
// SSSE3以上では _mm[256]_alignr_epi8 を使って実装するのが簡単だが，
// AVX2ではconcatinationに先立って128bit単位のunpackが行なわれてしまう．

//! ベクトルの左回転を行なう．
template <class T> static inline vec<T>
rotate_l(vec<T> x)			{return shuffle<2, 1, 0, 3>(x);}
//! ベクトルの右回転を行なう．
template <class T> static inline vec<T>
rotate_r(vec<T> x)			{return shuffle<0, 3, 2, 1>(x);}
//! ベクトルの逆転を行なう．
template <class T> static inline vec<T>
reverse(vec<T> x)			{return shuffle<0, 1, 2, 3>(x);}

#define MM_ROTATE_REVERSE_4(type)					\
    template <> inline vec<type>					\
    rotate_l(vec<type> x)	{return shuffle<2, 1, 0, 3>(x, x);}	\
    template <> inline vec<type>					\
    rotate_r(vec<type> x)	{return shuffle<0, 3, 2, 1>(x, x);}	\
    template <> inline vec<type>					\
    reverse(vec<type> x)	{return shuffle<0, 1, 2, 3>(x, x);}

#if defined(AVX)
  MM_ROTATE_REVERSE_4(double)
#elif defined(SSE)
  MM_ROTATE_REVERSE_4(float)
#  if defined(SSE2)
    template <> inline F64vec
    rotate_l(F64vec x)			{return shuffle<0, 1>(x, x);}
    template <> inline F64vec
    rotate_r(F64vec x)			{return rotate_l(x);}
    template <> inline F64vec
    reverse(F64vec x)			{return rotate_l(x);}
#  endif
#endif

#undef MM_ROTATE_REVERSE_4

/************************************************************************
*  Bitwise shift operators						*
************************************************************************/
//! 整数ベクトルの左シフトを行う．
/*!
  \param x	整数ベクトル
  \param n	シフトするビット数
  \return	シフト後の整数ベクトル
*/
template <class T> static vec<T>	operator <<(vec<T> x, int n)	;

//! 整数ベクトルの算術右シフトを行なう．
/*!
  \param x	整数ベクトル
  \param n	シフトするビット数
  \return	シフト後の整数ベクトル
*/
template <class T> static vec<T>	operator >>(vec<T> x, int n)	;

#define MM_LOGICAL_SHIFT_LEFT(type)					\
    MM_FUNC(vec<type> operator <<(vec<type> x, int n),			\
	    slli, (x, n), void, type, MM_SIGNED)
#define MM_LOGICAL_SHIFT_RIGHT(type)					\
    MM_FUNC(vec<type> operator >>(vec<type> x, int n),			\
	    srli, (x, n), void, type, MM_SIGNED)
#define MM_NUMERIC_SHIFT_RIGHT(type)					\
    MM_FUNC(vec<type> operator >>(vec<type> x, int n),			\
	    srai, (x, n), void, type, MM_SIGNED)

MM_LOGICAL_SHIFT_LEFT(int16_t)
MM_LOGICAL_SHIFT_LEFT(int32_t)
MM_LOGICAL_SHIFT_LEFT(u_int16_t)
MM_LOGICAL_SHIFT_LEFT(u_int32_t)
MM_LOGICAL_SHIFT_LEFT(u_int64_t)

MM_NUMERIC_SHIFT_RIGHT(int16_t)
MM_NUMERIC_SHIFT_RIGHT(int32_t)
MM_LOGICAL_SHIFT_RIGHT(u_int16_t)
MM_LOGICAL_SHIFT_RIGHT(u_int32_t)
MM_LOGICAL_SHIFT_RIGHT(u_int64_t)

#undef MM_LOGICAL_SHIFT_LEFT
#undef MM_LOGICAL_SHIFT_RIGHT
#undef MM_NUMERIC_SHIFT_RIGHT

/************************************************************************
*  Type conversion operators						*
************************************************************************/
//! T型ベクトルの下位半分をS型ベクトルに型変換する．
/*!
  整数ベクトル間の変換の場合，SのサイズはTの2倍である．また，S, Tは
  符号付き／符号なしのいずれでも良いが，符号付き -> 符号なしの変換はできない．
  \param x	変換されるベクトル
  \return	変換されたベクトル
*/
template <class S, class T> static vec<S>	cvt(vec<T> x)		;

//! T型ベクトルの上位半分をS型ベクトルに型変換する．
/*!
  整数ベクトル間の変換の場合，SのサイズはTの2倍である．また，S, Tは
  符号付き／符号なしのいずれでも良いが，符号付き -> 符号なしの変換はできない．
  \param x	変換されるベクトル
  \return	変換されたベクトル
*/
template <class S, class T> static vec<S>	cvt_high(vec<T> x)	;

//! 2つのT型整数ベクトルをより小さなS型整数ベクトルに型変換する．
/*!
  Tは符号付き整数型，SはTの半分のサイズを持つ符号付き／符号なし整数型
  である．Sが符号付き／符号なしのいずれの場合も飽和処理が行われる．
  \param x	変換されるベクトル
  \param y	変換されるベクトル
  \return	xが変換されたものを下位，yが変換されたものを上位に
		配したベクトル
*/
template <class S, class T> static vec<S>	cvt(vec<T> x, vec<T> y)	;

// [1] 整数ベクトル間の変換
#if defined(SSE4)
#  if defined(AVX2)
#    define MM_CVTUP(from, to)						\
      template <> inline vec<to>					\
      cvt<to>(vec<from> x)						\
      {									\
	  return MM_MNEMONIC(cvt, _mm256_, MM_SUFFIX(from),		\
			     MM_SIGNED(to))(_mm256_castsi256_si128(x));	\
      }
#    define MM_CVTHI(from, to)						\
      template <> inline vec<to>					\
      cvt_high<to>(vec<from> x)						\
      {									\
	  return MM_MNEMONIC(cvt, _mm256_, MM_SUFFIX(from),		\
			     MM_SIGNED(to))(				\
				 _mm256_extractf128_si256(x, 0x1));	\
      }
#  else	// SSE4 && !AVX2
#    define MM_CVTUP(from, to)						\
      template <> inline vec<to>					\
      cvt<to>(vec<from> x)						\
      {									\
	  return MM_MNEMONIC(cvt, _mm_,					\
			     MM_SUFFIX(from), MM_SIGNED(to))(x);	\
      }
#    define MM_CVTHI(from, to)						\
      template <> inline vec<to>					\
      cvt_high<to>(vec<from> x)						\
      {									\
	  return cvt<to>(shift_r<vec<from>::size/2>(x));		\
      }
#  endif
  MM_CVTUP(int8_t,    int16_t)		// s_char -> short
  MM_CVTHI(int8_t,    int16_t)		// s_char -> short
  MM_CVTUP(int8_t,    int32_t)		// s_char -> int
  MM_CVTUP(int8_t,    u_int64_t)	// s_char -> u_long
  
  MM_CVTUP(int16_t,   int32_t)		// short  -> int
  MM_CVTHI(int16_t,   int32_t)		// short  -> int
  MM_CVTUP(int16_t,   u_int64_t)	// short  -> u_long
  
  MM_CVTUP(int32_t,   u_int64_t)	// int    -> u_long
  MM_CVTHI(int32_t,   u_int64_t)	// int    -> u_long

  MM_CVTUP(u_int8_t,  int16_t)		// u_char -> short
  MM_CVTHI(u_int8_t,  int16_t)		// u_char -> short
  MM_CVTUP(u_int8_t,  u_int16_t)	// u_char -> u_short
  MM_CVTHI(u_int8_t,  u_int16_t)	// u_char -> u_short
  MM_CVTUP(u_int8_t,  int32_t)		// u_char -> int
  MM_CVTUP(u_int8_t,  u_int64_t)	// u_char -> u_long
  
  MM_CVTUP(u_int16_t, int32_t)		// u_short -> int
  MM_CVTHI(u_int16_t, int32_t)		// u_short -> int
  MM_CVTUP(u_int16_t, u_int32_t)	// u_short -> u_int
  MM_CVTHI(u_int16_t, u_int32_t)	// u_short -> u_int
  MM_CVTUP(u_int16_t, u_int64_t)	// u_short -> u_long
  
  MM_CVTUP(u_int32_t, u_int64_t)	// u_int -> u_long
  MM_CVTHI(u_int32_t, u_int64_t)	// u_int -> u_long

#  undef MM_CVTUP
#  undef MM_CVTHI
#else	// !SSE4
#  define MM_CVTUP_I(from, to)						\
    template <> inline vec<to>						\
    cvt<to>(vec<from> x)						\
    {									\
	return cast<to>(dup<0>(x)) >> 8*vec<from>::value_size;		\
    }									\
    template <> inline vec<to>						\
    cvt_high(vec<from> x)						\
    {									\
	return cast<to>(dup<1>(x)) >> 8*vec<from>::value_size;		\
    }
#  define MM_CVTUP_UI(from, to)						\
    template <> inline vec<to>						\
    cvt<to>(vec<from> x)						\
    {									\
	return cast<to>(unpack_low(x, zero<from>()));			\
    }									\
    template <> inline vec<to>						\
    cvt_high(vec<from> x)						\
    {									\
	return cast<to>(unpack_high(x, zero<from>()));			\
    }

  MM_CVTUP_I(int8_t,     int16_t)	// s_char  -> short
  MM_CVTUP_I(int16_t,    int32_t)	// short   -> int
  MM_CVTUP_I(int32_t,    u_int64_t)	// int	   -> u_long

  MM_CVTUP_UI(u_int8_t,  int16_t)	// u_char  -> short
  MM_CVTUP_UI(u_int8_t,  u_int16_t)	// u_char  -> u_short
  MM_CVTUP_UI(u_int16_t, int32_t)	// u_short -> int
  MM_CVTUP_UI(u_int16_t, u_int32_t)	// u_short -> u_int
  MM_CVTUP_UI(u_int32_t, u_int64_t)	// u_int   -> u_long

#  undef MM_CVTUP_I
#  undef MM_CVTUP_UI
#endif

#if defined(AVX2)
#  define MM_CVTDOWN_I(from, to)					\
    template <> inline vec<to>						\
    cvt(vec<from> x, vec<from> y)					\
    {									\
	return MM_MNEMONIC(packs, _mm256_, , MM_SUFFIX(from))(		\
			       _mm256_permute2f128_si256(x, y, 0x20),	\
			       _mm256_permute2f128_si256(x, y, 0x31));	\
    }
#  define MM_CVTDOWN_UI(from, to)					\
    template <> inline vec<to>						\
    cvt(vec<from> x, vec<from> y)					\
    {									\
	return MM_MNEMONIC(packus, _mm256_, , MM_SUFFIX(from))(		\
			       _mm256_permute2f128_si256(x, y, 0x20),	\
			       _mm256_permute2f128_si256(x, y, 0x31));	\
    }
#else
#  define MM_CVTDOWN_I(from, to)					\
    MM_FUNC(vec<to> cvt<to>(vec<from> x, vec<from> y),			\
	    packs, (x, y), void, from, MM_SIGNED)
#  define MM_CVTDOWN_UI(from, to)					\
    MM_FUNC(vec<to> cvt<to>(vec<from> x, vec<from> y),			\
	    packus, (x, y), void, from, MM_SIGNED)
#endif

#define _mm_packus_pi16	_mm_packs_pu16	// 不適切な命名をSSE2に合わせて修正

MM_CVTDOWN_I(int16_t,  int8_t)		// short -> s_char
MM_CVTDOWN_I(int32_t,  int16_t)		// int   -> short
MM_CVTDOWN_UI(int16_t, u_int8_t)	// short -> u_char
#if defined(SSE4)
  MM_CVTDOWN_UI(int32_t, u_int16_t)	// int -> u_short
#endif

#undef MM_CVTDOWN_I
#undef MM_CVTDOWN_UI

// [2] 整数ベクトルと浮動小数点数ベクトル間の変換
#define MM_CVT(from, to)						\
    MM_FUNC(vec<to> cvt<to>(vec<from> x), cvt, (x), from, to, MM_SUFFIX)
#define MM_CVT_2(type0, type1)						\
    MM_CVT(type0, type1)						\
    MM_CVT(type1, type0)

#if defined(AVX)
#  if defined(AVX2)
    MM_CVT_2(int32_t, float)		// int   <-> float

    template <> inline F64vec		// int    -> double
    cvt<double>(Is32vec x)
    {
	return _mm256_cvtepi32_pd(_mm256_castsi256_si128(x));			
    }

    template <> inline Is32vec		// double -> int
    cvt<int32_t>(F64vec x)		
    {
	return _mm256_castsi128_si256(_mm256_cvtpd_epi32(x));
    }

#    define MM_CVTI_F(itype, ftype)					\
      template <> inline vec<ftype>					\
      cvt<ftype>(vec<itype> x)						\
      {									\
	  return cvt<ftype>(cvt<int32_t>(x));				\
      }
#  else		// AVX && !AVX2
    template <> inline F32vec		// int    -> float
    cvt<float>(Is32vec x)
    {
	return _mm256_cvtepi32_ps(_mm256_castsi128_si256(x));			
    }

    template <> inline Is32vec		// float  -> int
    cvt<int32_t>(F32vec x)		
    {
	return _mm256_castsi256_si128(_mm256_cvtps_epi32(x));
    }

    MM_CVT(int32_t, double)		// int    -> double

    template <> inline Is32vec		// double -> int
    cvt<int32_t>(F64vec x)
    {
	return _mm256_cvtpd_epi32(x);
    }

    template <> inline F32vec
    cvt<float>(Is32vec x, Is32vec y)	// 2*int  -> float
    {
	return _mm256_cvtepi32_ps(
		   _mm256_insertf128_si256(
		       _mm256_insertf128_si256(_mm256_setzero_si256(),
					       x, 0x0),
		       y, 0x1));
    }

#    define MM_CVTI_F(itype, ftype)					\
      template <> inline vec<ftype>					\
      cvt<ftype>(vec<itype> x)						\
      {									\
	  return MM_MNEMONIC(cvt, _mm256_, epi32, MM_SUFFIX(ftype))(	\
		     _mm256_insertf128_si256(				\
			 _mm256_insertf128_si256(_mm256_setzero_si256(),\
						 cvt<int32_t>(x), 0x0),	\
			 cvt<int32_t>(shift_r<4>(x)), 0x1));		\
      }
#  endif

  MM_CVTI_F(int8_t,    float)		// s_char  -> float
  MM_CVTI_F(int16_t,   float)		// short   -> float
  MM_CVTI_F(u_int8_t,  float)		// u_char  -> float
  MM_CVTI_F(u_int16_t, float)		// u_short -> float

#  undef MM_CVTI_F
#elif defined(SSE2)
#  define MM_CVTI_F(itype, suffix)					\
    template <> inline vec<float>					\
    cvt<float>(vec<itype> x)						\
    {									\
	return MM_MNEMONIC(cvt, _mm_, suffix, ps)(_mm_movepi64_pi64(x));\
    }
#  define MM_CVTF_I(itype, suffix)					\
    template <> inline vec<itype>					\
    cvt<itype>(vec<float> x)						\
    {									\
	return _mm_movpi64_epi64(MM_MNEMONIC(cvt, _mm_, ps, suffix)(x));\
    }
#  define MM_CVT_2FI(itype, suffix)					\
    MM_CVTI_F(itype, suffix)						\
    MM_CVTF_I(itype, suffix)

  MM_CVT_2FI(int8_t,   pi8)		// s_char <-> float
  MM_CVT_2FI(int16_t,  pi16)		// short  <-> float
  MM_CVT_2(int32_t, float)		// int    <-> float
  MM_CVTI_F(u_int8_t,  pu8)		// u_char  -> float
  MM_CVTI_F(u_int16_t, pu16)		// u_short -> float

  MM_CVT_2(int32_t, double)		// int    <-> double

#  undef MM_CVTI_F
#  undef MM_CVTF_I
#  undef MM_CVT_2FI
#elif defined(SSE)
  MM_CVT_2(int8_t,  float)		// s_char <-> float
  MM_CVT_2(int16_t, float)		// short  <-> float
  MM_FUNC(F32vec cvt<float>(Is32vec x), cvt,
	  (zero<float>(), x), int32_t, float, MM_SUFFIX)  // int -> float
  MM_CVT(float,	    int32_t)		// float   -> int
  MM_CVT(u_int8_t,  float)		// u_char  -> float
  MM_CVT(u_int16_t, float)		// u_short -> float

  template <> inline F32vec
  cvt<float>(Is32vec x, Is32vec y)	// 2*int  -> float
  {
      return _mm_cvtpi32x2_ps(x, y);
  }
#endif
  
// [3] 浮動小数点数ベクトル間の変換
#if defined(AVX)
  template <> F64vec
  cvt<double>(F32vec x)			// float -> double
  {
      return _mm256_cvtps_pd(_mm256_castps256_ps128(x));
  }

template <> F32vec			// double -> float
  cvt<float>(F64vec x)
  {
      return _mm256_castps128_ps256(_mm256_cvtpd_ps(x));
  }
#elif defined(SSE2)
  MM_CVT_2(float, double)		// float <-> double
#endif
  
#undef MM_CVT
#undef MM_CVT_2

/************************************************************************
*  Mask conversion operators						*
************************************************************************/
//! T型マスクベクトルの下位半分をS型マスクベクトルに型変換する．
/*!
  整数ベクトル間の変換の場合，SのサイズはTの倍である．また，S, Tは
  符号付き／符号なしのいずれでも良い．
  \param x	変換されるマスクベクトル
  \return	変換されたマスクベクトル
*/
template <class S, class T> static vec<S>
cvt_mask(vec<T> x)							;

//! T型マスクベクトルの上位半分をS型マスクベクトルに型変換する．
/*!
  整数ベクトル間の変換の場合，SのサイズはTの倍である．また，S, Tは
  符号付き／符号なしのいずれでも良い．
  \param x	変換されるマスクベクトル
  \return	変換されたマスクベクトル
*/
template <class S, class T> static vec<S>
cvt_mask_high(vec<T> x)							;

//! 2つのT型整数マスクベクトルをより小さなS型整数マスクベクトルに型変換する．
/*!
  SのサイズはTの倍である．また，S, Tは符号付き／符号なしのいずれでも良い．
  \param x	変換されるマスクベクトル
  \param y	変換されるマスクベクトル
  \return	xが変換されたものを下位，yが変換されたものを上位に
		配したマスクベクトル
*/
template <class S, class T> static vec<S>
cvt_mask(vec<T> x, vec<T> y)						;

// [1] 整数ベクトル間のマスク変換
#if defined(AVX2)
#  define MM_CVTUP_MASK(from, to)					\
    template <> inline vec<to>						\
    cvt_mask(vec<from> x)						\
    {									\
	return MM_MNEMONIC(cvt, _mm256_,				\
			   MM_SIGNED(from), MM_SIGNED(to))(		\
		   _mm256_castsi256_si128(x));				\
    }									\
    template <> inline vec<to>						\
    cvt_mask_high(vec<from> x)						\
    {									\
	return MM_MNEMONIC(cvt, _mm256_,				\
			   MM_SIGNED(from), MM_SIGNED(to))(		\
		   _mm256_extractf128_si256(x, 0x1));			\
    }
#  define MM_CVTDOWN_MASK(from, to)					\
    template <> inline vec<to>						\
    cvt_mask(vec<from> x, vec<from> y)					\
    {									\
	return MM_MNEMONIC(packs, _mm256_, , MM_SIGNED(from))(		\
			       _mm256_permute2f128_si256(x, y, 0x20),	\
			       _mm256_permute2f128_si256(x, y, 0x31));	\
    }
#else
#  define MM_CVTUP_MASK(from, to)					\
    MM_FUNC(vec<to> cvt_mask(vec<from> x),				\
	    unpacklo, (x, x), void, from, MM_SIGNED)			\
    MM_FUNC(vec<to> cvt_mask_high(vec<from> x),				\
	    unpackhi, (x, x), void, from, MM_SIGNED)
#  define MM_CVTDOWN_MASK(from, to)					\
    MM_FUNC(vec<to> cvt_mask(vec<from> x, vec<from> y),			\
	    packs, (x, y), void, from, MM_SIGNED)
#endif
#define MM_CVT_MASK(type0, type1)					\
    MM_CVTUP_MASK(type0, type1)						\
    MM_CVTDOWN_MASK(type1, type0)

MM_CVT_MASK(int8_t,    int16_t)		// s_char  <-> short
MM_CVT_MASK(int8_t,    u_int16_t)	// s_char  <-> u_short
MM_CVT_MASK(int16_t,   int32_t)		// short   <-> int
MM_CVT_MASK(int16_t,   u_int32_t)	// short   <-> u_int
MM_CVT_MASK(u_int8_t,  int16_t)		// u_char  <-> short
MM_CVT_MASK(u_int8_t,  u_int16_t)	// u_char  <-> u_short
MM_CVT_MASK(u_int16_t, int32_t)		// u_short <-> int
MM_CVT_MASK(u_int16_t, u_int32_t)	// u_short <-> u_int
MM_CVTUP_MASK(int32_t,   u_int64_t)	// int      -> u_long
MM_CVTUP_MASK(u_int32_t, u_int64_t)	// u_int    -> u_long

#undef MM_CVTUP_MASK
#undef MM_CVTDOWN_MASK
#undef MM_CVT_MASK
    
// [2] 整数ベクトルと浮動小数点数ベクトル間のマスク変換
#if defined(SSE2)
// int, u_int, short, u_short, s_char, u_char -> float
template <> inline F32vec
cvt_mask<float>(Is32vec x)  {return cast<float>(x);}
template <> inline F32vec
cvt_mask<float>(Iu32vec x)  {return cast<float>(x);}
template <> inline F32vec
cvt_mask<float>(Is16vec x)  {return cvt_mask<float>(cvt_mask<int32_t>(x));}
template <> inline F32vec
cvt_mask<float>(Iu16vec x)  {return cvt_mask<float>(cvt_mask<u_int32_t>(x));}
template <> inline F32vec
cvt_mask<float>(Is8vec x)   {return cvt_mask<float>(cvt_mask<int16_t>(x));}
template <> inline F32vec
cvt_mask<float>(Iu8vec x)   {return cvt_mask<float>(cvt_mask<u_int16_t>(x));}

// float -> int, u_int
template <> inline Is32vec
cvt_mask<int32_t>(F32vec x)	{return cast<int32_t>(x);}
template <> inline Iu32vec
cvt_mask<u_int32_t>(F32vec x)	{return cast<u_int32_t>(x);}

// u_int64, int, u_int, short, u_short, s_char, u_char -> double
template <> inline F64vec
cvt_mask<double>(I64vec x)  {return cast<double>(x);}
template <> inline F64vec
cvt_mask<double>(Is32vec x) {return cvt_mask<double>(cvt_mask<u_int64_t>(x));}
template <> inline F64vec
cvt_mask<double>(Iu32vec x) {return cvt_mask<double>(cvt_mask<u_int64_t>(x));}
template <> inline F64vec
cvt_mask<double>(Is16vec x) {return cvt_mask<double>(cvt_mask<int32_t>(x));}
template <> inline F64vec
cvt_mask<double>(Iu16vec x) {return cvt_mask<double>(cvt_mask<u_int32_t>(x));}
template <> inline F64vec
cvt_mask<double>(Is8vec x)  {return cvt_mask<double>(cvt_mask<int16_t>(x));}
template <> inline F64vec
cvt_mask<double>(Iu8vec x)  {return cvt_mask<double>(cvt_mask<u_int16_t>(x));}

// double -> u_int64
template <> inline I64vec
cvt_mask<u_int64_t>(F64vec x)	{return cast<u_int64_t>(x);}
#endif

/************************************************************************
*  Logical operators							*
************************************************************************/
template <class T> static vec<T>	operator &(vec<T> x, vec<T> y)	;
template <class T> static vec<T>	operator |(vec<T> x, vec<T> y)	;
template <class T> static vec<T>	operator ^(vec<T> x, vec<T> y)	;
template <class T> static vec<T>	andnot(vec<T> x, vec<T> y)	;

#define MM_LOGICALS(type)						\
    MM_BASE_FUNC_2(operator &, and,    type)				\
    MM_BASE_FUNC_2(operator |, or,     type)				\
    MM_BASE_FUNC_2(operator ^, xor,    type)				\
    MM_BASE_FUNC_2(andnot,     andnot, type)

MM_LOGICALS(int8_t)
MM_LOGICALS(int16_t)
MM_LOGICALS(int32_t)
MM_LOGICALS(u_int8_t)
MM_LOGICALS(u_int16_t)
MM_LOGICALS(u_int32_t)
MM_LOGICALS(u_int64_t)

#if defined(SSE)
  MM_LOGICALS(float)
#endif
#if defined(SSE2)
  MM_LOGICALS(double)
#endif

#undef MM_LOGICALS

/************************************************************************
*  Lookup								*
************************************************************************/
#if defined(AVX2)
#  define MM_LOOKUP(type, base)						\
    template <class S> static inline vec<type>				\
    lookup(const S* p, vec<type> idx)					\
    {									\
	return cvt<type>(lookup(p, cvt<base>(idx)),			\
			 lookup(p, cvt_high<base>(idx)));		\
    }

  template <class S> static inline vec<int>
  lookup(const S* p, vec<int> i)
  {
      const u_int	n = 8*(sizeof(int) - sizeof(S));
      if (type_traits<S>::is_signed)
      {
	  vec<int>	val(_mm256_i32gather_epi32((const int*)p,
						   i, sizeof(S)));
	  return (val << n) >> n;
      }
      else
      {
	  vec<u_int>	val(_mm256_i32gather_epi32((const int*)p,
						   i, sizeof(S)));
	  return cast<int>((val << n) >> n);
      }
  }

  MM_LOOKUP(short,   int)
  MM_LOOKUP(u_short, int);
  MM_LOOKUP(s_char,  short)
  MM_LOOKUP(u_char,  short)

#  undef MM_LOOKUP
#else	// !AVX2
#  define MM_LOOKUP4(type)						\
    template <class S> static inline vec<type>				\
    lookup(const S* p, vec<type> idx)					\
    {									\
	return vec<type>(p[extract<3>(idx)], p[extract<2>(idx)],	\
			 p[extract<1>(idx)], p[extract<0>(idx)]);	\
    }
#  if defined(SSE2)
#    define MM_LOOKUP8(type)						\
    template <class S> static inline vec<type>				\
    lookup(const S* p, vec<type> idx)					\
    {									\
	return vec<type>(p[extract<7>(idx)], p[extract<6>(idx)],	\
			 p[extract<5>(idx)], p[extract<4>(idx)],	\
			 p[extract<3>(idx)], p[extract<2>(idx)],	\
			 p[extract<1>(idx)], p[extract<0>(idx)]);	\
    }
#  else		// !SSE2
#    define MM_LOOKUP8(type)						\
    template <class S> static inline vec<type>				\
    lookup(const S* p, vec<type> idx)					\
    {									\
	const Is16vec	idx_lo = cvt<int16_t>(idx),			\
			idx_hi = cvt_high<int16_t>(idx);		\
	return vec<type>(p[extract<3>(idx_hi)], p[extract<2>(idx_hi)],	\
			 p[extract<1>(idx_hi)], p[extract<0>(idx_hi)],	\
			 p[extract<3>(idx_lo)], p[extract<2>(idx_lo)],	\
			 p[extract<1>(idx_lo)], p[extract<0>(idx_lo)]);	\
    }
#  endif
#  if defined(SSE4)
#    define MM_LOOKUP16(type)						\
    template <class S> static inline vec<type>				\
    lookup(const S* p, vec<type> idx)					\
    {									\
	return vec<type>(p[extract<15>(idx)], p[extract<14>(idx)],	\
			 p[extract<13>(idx)], p[extract<12>(idx)],	\
			 p[extract<11>(idx)], p[extract<10>(idx)],	\
			 p[extract< 9>(idx)], p[extract< 8>(idx)],	\
			 p[extract< 7>(idx)], p[extract< 6>(idx)],	\
			 p[extract< 5>(idx)], p[extract< 4>(idx)],	\
			 p[extract< 3>(idx)], p[extract< 2>(idx)],	\
			 p[extract< 1>(idx)], p[extract< 0>(idx)]);	\
    }
#  else		// !SSE4
#    define MM_LOOKUP16(type)						\
    template <class S> static inline vec<type>				\
    lookup(const S* p, vec<type> idx)					\
    {									\
	const Is16vec	idx_lo = cvt<int16_t>(idx),			\
			idx_hi = cvt_high<int16_t>(idx);		\
	return vec<type>(p[extract<7>(idx_hi)], p[extract<6>(idx_hi)],	\
			 p[extract<5>(idx_hi)], p[extract<4>(idx_hi)],	\
			 p[extract<3>(idx_hi)], p[extract<2>(idx_hi)],	\
			 p[extract<1>(idx_hi)], p[extract<0>(idx_hi)],	\
			 p[extract<7>(idx_lo)], p[extract<6>(idx_lo)],	\
			 p[extract<5>(idx_lo)], p[extract<4>(idx_lo)],	\
			 p[extract<3>(idx_lo)], p[extract<2>(idx_lo)],	\
			 p[extract<1>(idx_lo)], p[extract<0>(idx_lo)]);	\
    }
#  endif
#  if defined(SSE2)
  MM_LOOKUP16(int8_t)
  MM_LOOKUP16(u_int8_t)
  MM_LOOKUP8(int16_t)
  MM_LOOKUP8(u_int16_t)
#    if defined(SSE4)
    MM_LOOKUP4(u_int32_t)
    MM_LOOKUP4(int32_t)
#    endif
#  else		// !SSE2
  MM_LOOKUP8(int8_t)
  MM_LOOKUP8(u_int8_t)
  MM_LOOKUP4(int16_t)
  MM_LOOKUP4(u_int16_t)
#  endif
#  undef MM_LOOKUP4
#  undef MM_LOOKUP8
#  undef MM_LOOKUP16
#endif
    
/************************************************************************
*  Selection								*
************************************************************************/
//! 2つのベクトル中の成分のいずれかをマスク値に応じて選択する．
/*!
 \param mask	マスク
 \param x	ベクトル
 \param y	ベクトル
 \return	maskにおいて1が立っている成分はxから，そうでない成分は
		yからそれぞれ選択して生成されたベクトル
*/
template <class T> static inline vec<T>
select(vec<T> mask, vec<T> x, vec<T> y)
{
    return (mask & x) | andnot(mask, y);
}

#if defined(SSE4)
#  define MM_SELECT(type)						\
    template <> inline vec<type>					\
    select(vec<type> mask, vec<type> x, vec<type> y)			\
    {									\
	   return MM_MNEMONIC(blendv, MM_PREFIX(type), ,		\
			      MM_SIGNED(int8_t))(y, x, mask);		\
    }
#  define MM_SELECT_F(type)						\
    MM_FUNC(vec<type> select(vec<type> mask, vec<type> x, vec<type> y),	\
	    blendv, (y, x, mask), void, type, MM_BASE)

  MM_SELECT(int8_t)
  MM_SELECT(int16_t)
  MM_SELECT(int32_t)
  MM_SELECT(u_int8_t)
  MM_SELECT(u_int16_t)
  MM_SELECT(u_int32_t)
  MM_SELECT(u_int64_t)

  MM_SELECT_F(float)
  MM_SELECT_F(double)
  
#  undef MM_SELECT
#  undef MM_SELECT_F
#endif

/************************************************************************
*  Compare operators							*
************************************************************************/
template <class T> static vec<T>	operator ==(vec<T> x, vec<T> y)	;
template <class T> static vec<T>	operator > (vec<T> x, vec<T> y)	;
template <class T> static vec<T>	operator < (vec<T> x, vec<T> y)	;
template <class T> static vec<T>	operator !=(vec<T> x, vec<T> y)	;
template <class T> static vec<T>	operator >=(vec<T> x, vec<T> y)	;
template <class T> static vec<T>	operator <=(vec<T> x, vec<T> y)	;

// MMX, SSE, AVX2 には整数に対する cmplt ("less than") がない！
#define MM_COMPARES(type)						\
    MM_NUMERIC_FUNC_2( operator ==, cmpeq, type)			\
    MM_NUMERIC_FUNC_2( operator >,  cmpgt, type)			\
    MM_NUMERIC_FUNC_2R(operator <,  cmpgt, type)

// 符号なし数に対しては等値性チェックしかできない！
MM_FUNC_2(operator ==, cmpeq, u_int8_t)
MM_FUNC_2(operator ==, cmpeq, u_int16_t)
MM_FUNC_2(operator ==, cmpeq, u_int32_t)
#if defined(SSE4)
  MM_FUNC_2(operator ==, cmpeq, u_int64_t)
#endif

MM_COMPARES(int8_t)
MM_COMPARES(int16_t)
MM_COMPARES(int32_t)

#if defined(AVX)	// AVX の浮動小数点数比較演算子はパラメータ形式
#  define MM_COMPARE_F(func, type, opcode)				\
    MM_FUNC(vec<type> func(vec<type> x, vec<type> y),			\
	    cmp, (x, y, opcode), void, type, MM_SUFFIX)
#  define MM_COMPARES_F(type)						\
    MM_COMPARE_F(operator ==, type, _CMP_EQ_OQ)				\
    MM_COMPARE_F(operator >,  type, _CMP_GT_OS)				\
    MM_COMPARE_F(operator <,  type, _CMP_LT_OS)				\
    MM_COMPARE_F(operator !=, type, _CMP_NEQ_OQ)			\
    MM_COMPARE_F(operator >=, type, _CMP_GE_OS)				\
    MM_COMPARE_F(operator <=, type, _CMP_LE_OS)

  MM_COMPARES_F(float)
  MM_COMPARES_F(double)

#  undef MM_COMPARE_F
#  undef MM_COMPARES_F
#elif defined(SSE)
#  define MM_COMPARES_SUP(type)						\
    MM_NUMERIC_FUNC_2( operator !=, cmpneq, type)				\
    MM_NUMERIC_FUNC_2( operator >=, cmpge,  type)				\
    MM_NUMERIC_FUNC_2R(operator <=, cmpge,  type)

  MM_COMPARES(float)
  MM_COMPARES_SUP(float)
#  if defined(SSE2)
    MM_COMPARES(double)
    MM_COMPARES_SUP(double)
#  endif

#  undef MM_COMPARES_SUP
#endif

#undef MM_COMPARES

/************************************************************************
*  Arithmetic operators							*
************************************************************************/
template <class T> static vec<T>	operator +(vec<T> x, vec<T> y)	;
template <class T> static vec<T>	operator -(vec<T> x, vec<T> y)	;
template <class T> static vec<T>	operator *(vec<T> x, vec<T> y)	;
template <class T> static vec<T>	operator /(vec<T> x, vec<T> y)	;
template <class T> static vec<T>	operator -(vec<T> x)		;
template <class T> static vec<T>	sat_add(vec<T> x, vec<T> y)	;
template <class T> static vec<T>	sat_sub(vec<T> x, vec<T> y)	;
template <class T> static vec<T>	mulhi(vec<T> x, vec<T> y)	;
template <class T> static vec<T>	min(vec<T> x, vec<T> y)		;
template <class T> static vec<T>	max(vec<T> x, vec<T> y)		;
template <class T> static vec<T>	rcp(vec<T> x)			;
template <class T> static vec<T>	sqrt(vec<T> x)			;
template <class T> static vec<T>	rsqrt(vec<T> x)			;

template <class T> inline vec<T>
min(vec<T> x, vec<T> y)
{
    return select(x < y, x, y);
}

template <class T> inline vec<T>
max(vec<T> x, vec<T> y)
{
    return select(x > y, x, y);
}

template <class T> inline vec<T>
operator -(vec<T> x)
{
    return zero<T>() - x;
}

#define MM_ADD_SUB(type)						\
    MM_NUMERIC_FUNC_2(operator +, add, type)					\
    MM_NUMERIC_FUNC_2(operator -, sub, type)

// 符号なし数は，飽和演算によって operator [+|-] を定義する．
#define MM_ADD_SUB_U(type)						\
    MM_NUMERIC_FUNC_2(operator +, adds, type)					\
    MM_NUMERIC_FUNC_2(operator -, subs, type)

// 符号あり数は，飽和演算に sat_[add|sub] という名前を与える．
#define MM_SAT_ADD_SUB(type)						\
    MM_NUMERIC_FUNC_2(sat_add, adds, type)					\
    MM_NUMERIC_FUNC_2(sat_sub, subs, type)

#define MM_MIN_MAX(type)						\
    MM_NUMERIC_FUNC_2(min, min, type)						\
    MM_NUMERIC_FUNC_2(max, max, type)

// 加減算
MM_ADD_SUB(int8_t)
MM_ADD_SUB(int16_t)
MM_ADD_SUB(int32_t)
MM_ADD_SUB_U(u_int8_t)
MM_ADD_SUB_U(u_int16_t)
MM_SAT_ADD_SUB(int8_t)
MM_SAT_ADD_SUB(int16_t)
MM_SAT_ADD_SUB(u_int8_t)
MM_SAT_ADD_SUB(u_int16_t)

// 乗算
MM_NUMERIC_FUNC_2(operator *, mullo, int16_t)
MM_NUMERIC_FUNC_2(mulhi,      mulhi, int16_t)

#if defined(SSE)
  // 加減算
  MM_ADD_SUB(float)

  // 乗除算
  MM_NUMERIC_FUNC_2(operator *, mul, float)
  MM_NUMERIC_FUNC_2(operator /, div, float)

  // Min/Max
  MM_MIN_MAX(u_int8_t)
  MM_MIN_MAX(int16_t)
  MM_MIN_MAX(float)

  // その他
  MM_NUMERIC_FUNC_1(sqrt,  sqrt,  float)
  MM_NUMERIC_FUNC_1(rsqrt, rsqrt, float)
  MM_NUMERIC_FUNC_1(rcp,   rcp,   float)
#endif

#if defined(SSE2)
  // 加減算
  MM_ADD_SUB(double)

  // 乗除算
  MM_NUMERIC_FUNC_2(operator *, mul, u_int32_t)
  MM_NUMERIC_FUNC_2(operator *, mul, double)
  MM_NUMERIC_FUNC_2(operator /, div, double)

  // Min/Max
  MM_MIN_MAX(double)

  // その他
  MM_NUMERIC_FUNC_1(sqrt, sqrt, double)
#endif

#if defined(SSE4)
  // 乗算
  MM_NUMERIC_FUNC_2(operator *, mullo, int32_t)

  // Min/Max
  MM_MIN_MAX(int8_t)
  MM_MIN_MAX(int32_t)
  MM_MIN_MAX(u_int16_t)
  MM_MIN_MAX(u_int32_t)
#endif

#undef MM_ADD_SUB
#undef MM_ADD_SUB_U
#undef MM_SAT_ADD_SUB
#undef MM_MIN_MAX

/************************************************************************
*  "[Greater|Less] than or equal to" operators				*
************************************************************************/
template <class T> static inline vec<T>
operator >=(vec<T> x, vec<T> y)
{
    return max(x, y) == x;
}

template <class T> static inline vec<T>
operator <=(vec<T> x, vec<T> y)
{
    return y >= x;
}

/************************************************************************
*  Average values							*
************************************************************************/
template <class T> static inline vec<T>
avg(vec<T> x, vec<T> y)			{return (x + y) >> 1;}
template <class T> static inline vec<T>
sub_avg(vec<T> x, vec<T> y)		{return (x - y) >> 1;}

#if defined(SSE)
  MM_NUMERIC_FUNC_2(avg, avg, u_int8_t)
  MM_NUMERIC_FUNC_2(avg, avg, u_int16_t)
  template <> inline F32vec
  avg(F32vec x, F32vec y)		{return (x + y) * F32vec(0.5f);}
  template <> inline F32vec
  sub_avg(F32vec x, F32vec y)		{return (x - y) * F32vec(0.5f);}
#endif

#if defined(SSE2)
  template <> inline F64vec
  avg(F64vec x, F64vec y)		{return (x + y) * F64vec(0.5d);}
  template <> inline F64vec
  sub_avg(F64vec x, F64vec y)		{return (x - y) * F64vec(0.5d);}
#endif
  
/************************************************************************
*  Absolute values							*
************************************************************************/
template <class T> static inline vec<T>	abs(vec<T> x)	{return max(x, -x);}
template <> inline Iu8vec		abs(Iu8vec x)	{return x;}
template <> inline Iu16vec		abs(Iu16vec x)	{return x;}
template <> inline Iu32vec		abs(Iu32vec x)	{return x;}

#if defined(SSSE3)
  MM_NUMERIC_FUNC_1(abs, abs, int8_t)
  MM_NUMERIC_FUNC_1(abs, abs, int16_t)
  MM_NUMERIC_FUNC_1(abs, abs, int32_t)
#endif
  
/************************************************************************
*  Absolute differences							*
************************************************************************/
template <class T> inline vec<T>
diff(vec<T> x, vec<T> y)	{return select(x > y, x - y, y - x);}
template <> inline Is8vec
diff(Is8vec x, Is8vec y)	{return sat_sub(x, y) | sat_sub(y, x);}
template <> inline Iu8vec
diff(Iu8vec x, Iu8vec y)	{return sat_sub(x, y) | sat_sub(y, x);}
template <> inline Is16vec
diff(Is16vec x, Is16vec y)	{return sat_sub(x, y) | sat_sub(y, x);}
template <> inline Iu16vec
diff(Iu16vec x, Iu16vec y)	{return sat_sub(x, y) | sat_sub(y, x);}
  
/************************************************************************
*  SVML(Short Vector Math Library) functions				*
************************************************************************/
#if defined(SSE) && defined(USE_SVML)
template <class T> static vec<T>	erf(vec<T> x)			;
template <class T> static vec<T>	erfc(vec<T> x)			;

template <class T> static vec<T>	exp(vec<T> x)			;
template <class T> static vec<T>	cexp(vec<T> x)			;
template <class T> static vec<T>	exp2(vec<T> x)			;
template <class T> static vec<T>	pow(vec<T> x, vec<T> y)		;

template <class T> static vec<T>	log(vec<T> x)			;
template <class T> static vec<T>	log2(vec<T> x)			;
template <class T> static vec<T>	log10(vec<T> x)			;
template <class T> static vec<T>	clog(vec<T> x)			;

template <class T> static vec<T>	invsqrt(vec<T> x)		;
template <class T> static vec<T>	cbrt(vec<T> x)			;
template <class T> static vec<T>	invcbrt(vec<T> x)		;
template <class T> static vec<T>	csqrt(vec<T> x)			;

template <class T> static vec<T>	cos(vec<T> x)			;
template <class T> static vec<T>	sin(vec<T> x)			;
template <class T> static vec<T>	tan(vec<T> x)			;
template <class T> static vec<T>	sincos(typename
					       vec<T>::base_type* pcos,
					       vec<T> x)		;
template <class T> static vec<T>	acos(vec<T> x)			;
template <class T> static vec<T>	asin(vec<T> x)			;
template <class T> static vec<T>	atan(vec<T> x)			;
template <class T> static vec<T>	atan2(vec<T> x, vec<T> y)	;
template <class T> static vec<T>	cosh(vec<T> x)			;
template <class T> static vec<T>	sinh(vec<T> x)			;
template <class T> static vec<T>	tanh(vec<T> x)			;
template <class T> static vec<T>	acosh(vec<T> x)			;
template <class T> static vec<T>	asinh(vec<T> x)			;
template <class T> static vec<T>	atanh(vec<T> x)			;

#  if defined(SSE2)
  MM_NUMERIC_FUNC_1(erf,     erf,     double)
  MM_NUMERIC_FUNC_1(erfc,    erfc,    double)

  MM_NUMERIC_FUNC_1(exp,     exp,     double)
  MM_NUMERIC_FUNC_1(exp2,    exp2,    double)
  MM_NUMERIC_FUNC_2(pow,     pow,     double)

  MM_NUMERIC_FUNC_1(log,     log,     double)
  MM_NUMERIC_FUNC_1(log2,    log2,    double)
  MM_NUMERIC_FUNC_1(log10,   log10,   double)

  MM_NUMERIC_FUNC_1(invsqrt, invsqrt, double)
  MM_NUMERIC_FUNC_1(cbrt,    cbrt,    double)
  MM_NUMERIC_FUNC_1(invcbrt, invcbrt, double)

  MM_NUMERIC_FUNC_1(cos,     cos,     double)
  MM_NUMERIC_FUNC_1(sin,     sin,     double)
  MM_NUMERIC_FUNC_1(tan,     tan,     double)
  MM_FUNC(F64vec sincos(dvec_t* pcos, F64vec x),
	  sincos, (pcos, x),  void,   double)
  MM_NUMERIC_FUNC_1(acos,    acos,    double)
  MM_NUMERIC_FUNC_1(asin,    asin,    double)
  MM_NUMERIC_FUNC_1(atan,    atan,    double)
  MM_NUMERIC_FUNC_2(atan2,   atan2,   double)
  MM_NUMERIC_FUNC_1(cosh,    cosh,    double)
  MM_NUMERIC_FUNC_1(sinh,    sinh,    double)
  MM_NUMERIC_FUNC_1(tanh,    tanh,    double)
  MM_NUMERIC_FUNC_1(acosh,   acosh,   double)
  MM_NUMERIC_FUNC_1(asinh,   asinh,   double)
  MM_NUMERIC_FUNC_1(atanh,   atanh,   double)
#  endif
  MM_NUMERIC_FUNC_1(erf,     erf,     float)
  MM_NUMERIC_FUNC_1(erfc,    erfc,    float)

  MM_NUMERIC_FUNC_1(exp,     exp,     float)
  MM_NUMERIC_FUNC_1(cexp,    cexp,    float)
  MM_NUMERIC_FUNC_1(exp2,    exp2,    float)
  MM_NUMERIC_FUNC_2(pow,     pow,     float)

  MM_NUMERIC_FUNC_1(log,     log,     float)
  MM_NUMERIC_FUNC_1(log2,    log2,    float)
  MM_NUMERIC_FUNC_1(log10,   log10,   float)
  MM_NUMERIC_FUNC_1(clog,    clog,    float)

  MM_NUMERIC_FUNC_1(invsqrt, invsqrt, float)
  MM_NUMERIC_FUNC_1(cbrt,    cbrt,    float)
  MM_NUMERIC_FUNC_1(invcbrt, invcbrt, float)
  MM_NUMERIC_FUNC_1(csqrt,   csqrt,   float)

  MM_NUMERIC_FUNC_1(cos,     cos,     float)
  MM_NUMERIC_FUNC_1(sin,     sin,     float)
  MM_NUMERIC_FUNC_1(tan,     tan,     float)
  MM_FUNC(F32vec sincos(fvec_t* pcos, F32vec x),
	  sincos, (pcos, x),  void,  float)
  MM_NUMERIC_FUNC_1(acos,    acos,    float)
  MM_NUMERIC_FUNC_1(asin,    asin,    float)
  MM_NUMERIC_FUNC_1(atan,    atan,    float)
  MM_NUMERIC_FUNC_2(atan2,   atan2,   float)
  MM_NUMERIC_FUNC_1(cosh,    cosh,    float)
  MM_NUMERIC_FUNC_1(sinh,    sinh,    float)
  MM_NUMERIC_FUNC_1(tanh,    tanh,    float)
  MM_NUMERIC_FUNC_1(acosh,   acosh,   float)
  MM_NUMERIC_FUNC_1(asinh,   asinh,   float)
  MM_NUMERIC_FUNC_1(atanh,   atanh,   float)
#endif

/************************************************************************
*  Member funcsionts of vec<T>						*
************************************************************************/
#define MM_MEMBERS(type)						\
    inline vec<type>&							\
    vec<type>::flip_sign()		{return *this = -*this;}	\
    inline vec<type>&							\
    vec<type>::operator +=(vec<type> x)	{return *this = *this + x;}	\
    inline vec<type>&							\
    vec<type>::operator -=(vec<type> x)	{return *this = *this - x;}	\
    inline vec<type>&							\
    vec<type>::operator *=(vec<type> x)	{return *this = *this * x;}	\
    inline vec<type>&							\
    vec<type>::operator /=(vec<type> x)	{return *this = *this / x;}	\
    inline vec<type>&							\
    vec<type>::operator &=(vec<type> x)	{return *this = *this & x;}	\
    inline vec<type>&							\
    vec<type>::operator |=(vec<type> x)	{return *this = *this | x;}	\
    inline vec<type>&							\
    vec<type>::operator ^=(vec<type> x)	{return *this = *this ^ x;}	\
    inline vec<type>&							\
    vec<type>::andnot(vec<type> x)	{return *this = mm::andnot(x, *this);}

template <class T> inline vec<T>&
vec<T>::flip_sign()			{return *this = -*this;}
template <class T> inline vec<T>&
vec<T>::operator +=(vec<T> x)		{return *this = *this + x;}
template <class T> inline vec<T>&
vec<T>::operator -=(vec<T> x)		{return *this = *this - x;}
template <class T> inline vec<T>&
vec<T>::operator *=(vec<T> x)		{return *this = *this * x;}
template <class T> inline vec<T>&
vec<T>::operator &=(vec<T> x)		{return *this = *this & x;}
template <class T> inline vec<T>&
vec<T>::operator |=(vec<T> x)		{return *this = *this | x;}
template <class T> inline vec<T>&
vec<T>::operator ^=(vec<T> x)		{return *this = *this ^ x;}
template <class T> inline vec<T>&
vec<T>::andnot(vec<T> x)		{return *this = mm::andnot(x, *this);}

#if defined(SSE)
  MM_MEMBERS(float)
#endif
#if defined(SSE2)
  MM_MEMBERS(double)
#endif

#undef MM_MEMBERS

/************************************************************************
*  Control functions							*
************************************************************************/
inline void	empty()			{_mm_empty();}
    
/************************************************************************
*  Undefine macros							*
************************************************************************/
#undef MM_PREFIX
#undef MM_SUFFIX
#undef MM_BASE

#undef MM_PREFIX_int8_t
#undef MM_PREFIX_u_int8_t
#undef MM_PREFIX_int16_t
#undef MM_PREFIX_u_int16_t
#undef MM_PREFIX_int32_t
#undef MM_PREFIX_u_int32_t
#undef MM_PREFIX_u_int64_t
#undef MM_PREFIX_ivec_t

#undef MM_SUFFIX_int8_t
#undef MM_SUFFIX_u_int8_t
#undef MM_SUFFIX_int16_t
#undef MM_SUFFIX_u_int16_t
#undef MM_SUFFIX_int32_t
#undef MM_SUFFIX_u_int32_t
#undef MM_SUFFIX_u_int64_t
#undef MM_SUFFIX_ivec_t
#undef MM_SUFFIX_void

#undef MM_SIGNED_int8_t
#undef MM_SIGNED_u_int8_t
#undef MM_SIGNED_int16_t
#undef MM_SIGNED_u_int16_t
#undef MM_SIGNED_int32_t
#undef MM_SIGNED_u_int32_t
#undef MM_SIGNED_u_int64_t

#undef MM_BASE_int8_t
#undef MM_BASE_u_int8_t
#undef MM_BASE_int16_t
#undef MM_BASE_u_int16_t
#undef MM_BASE_int32_t
#undef MM_BASE_u_int32_t
#undef MM_BASE_u_int64_t
#undef MM_BASE_u_ivec_t

#if defined(SSE)
#  undef MM_PREFIX_float
#  undef MM_PREFIX_fvec_t
#  undef MM_SUFFIX_float
#  undef MM_SUFFIX_fvec_t
#  undef MM_SIGNED_float
#  undef MM_BASE_float
#  undef MM_BASE_fvec_t
#endif

#if defined(SSE2)
#  undef MM_PREFIX_double
#  undef MM_PREFIX_dvec_t
#  undef MM_SUFFIX_double
#  undef MM_SUFFIX_dvec_t
#  undef MM_SIGNED_double
#  undef MM_BASE_double
#  undef MM_BASE_dvec_t
#endif

#undef MM_CAT
#undef MM_MNEMONIC
#undef MM_TMPL_FUNC
#undef MM_FUNC
#undef MM_FUNC_2
#undef MM_NUMERIC_FUNC_1
#undef MM_NUMERIC_FUNC_2
#undef MM_NUMERIC_FUNC_2R

/************************************************************************
*  class load_iterator<T>						*
************************************************************************/
template <class T>
class load_iterator : public boost::iterator_adaptor<load_iterator<T>,
						     const T*,
						     vec<T>,
						     boost::use_default,
						     vec<T> >
{
  private:
    typedef boost::iterator_adaptor<load_iterator<T>,
				    const T*,
				    vec<T>,
				    boost::use_default,
				    vec<T> >		super;

  public:
    typedef typename super::difference_type	difference_type;
    typedef typename super::value_type		value_type;
    typedef typename super::pointer		pointer;
    typedef typename super::reference		reference;
    typedef typename super::iterator_category	iterator_category;

    friend class				boost::iterator_core_access;

  public:
    load_iterator(const T* p)	:super(p)	{}

  private:
    reference		dereference() const
			{
			    return loadu(super::base());
			}
    void		advance(difference_type n)
			{
			    super::base_reference() += n * value_type::size;
			}
    void		increment()
			{
			    super::base_reference() += value_type::size;
			}
    void		decrement()
			{
			    super::base_reference() -= value_type::size;
			}
    difference_type	distance_to(load_iterator iter) const
			{
			    return (iter.base() - super::base())
				 * value_type::size;
			}
};

template <class T> load_iterator<T>
make_load_iterator(const T* p)
{
    return load_iterator<T>(p);
}
    
/************************************************************************
*  class store_iterator<T>						*
************************************************************************/
namespace detail
{
    template <class T>
    class store_proxy
    {
      public:
	store_proxy(T* p)	:_p(p)		{}
	
	store_proxy&	operator =(vec<T> val)
			{
			    storeu(_p, val);
			    return *this;
			}
	
      private:
	T* const	_p;
    };
}

template <class T>
class store_iterator : public boost::iterator_adaptor<store_iterator<T>,
						      T*,
						      vec<T>,
						      boost::use_default,
						      detail::store_proxy<T> >
{
  private:
    typedef boost::iterator_adaptor<store_iterator<T>,
				    T*,
				    vec<T>,
				    boost::use_default,
				    detail::store_proxy<T> >	super;

  public:
    typedef typename super::difference_type	difference_type;
    typedef typename super::value_type		value_type;
    typedef typename super::pointer		pointer;
    typedef typename super::reference		reference;
    typedef typename super::iterator_category	iterator_category;

    friend class				boost::iterator_core_access;

  public:
    store_iterator(T* p)	:super(p)	{}

  private:
    reference		dereference() const
			{
			    return reference(super::base());
			}
    void		advance(difference_type n)
			{
			    super::base_reference() += n * value_type::size;
			}
    void		increment()
			{
			    super::base_reference() += value_type::size;
			}
    void		decrement()
			{
			    super::base_reference() -= value_type::size;
			}
    difference_type	distance_to(store_iterator iter) const
			{
			    return (iter.base() - super::base())
				 * value_type::size;
			}
};

template <class T> store_iterator<T>
make_store_iterator(T* p)
{
    return store_iterator<T>(p);
}

}
#endif

#endif	// !__mmInstructions_h
