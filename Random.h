/*
 *  $BJ?@.(B19$BG/!JFH!K;:6H5;=QAm9g8&5f=j(B $BCx:n8"=jM-(B
 *  
 *  $BAO:n<T!'?"<G=SIW(B
 *
 *  $BK\%W%m%0%i%`$O!JFH!K;:6H5;=QAm9g8&5f=j$N?&0w$G$"$k?"<G=SIW$,AO:n$7!$(B
 *  $BF1=j$,Cx:n8"$r=jM-$9$kHkL)>pJs$G$9!%Cx:n<T$K$h$k5v2D$J$7$K$3$N%W%m(B
 *  $B%0%i%`$rBh;0<T$X3+<(!$J#@=!$2~JQ!$;HMQ$9$kEy$NCx:n8"$r?/32$9$k9T0Y(B
 *  $B$r6X;_$7$^$9!%(B
 *  
 *  $B$3$N%W%m%0%i%`$K$h$C$F@8$8$k$$$+$J$kB;32$KBP$7$F$b!"Cx:n<T$O@UG$(B
 *  $B$rIi$$$^$;$s!#(B 
 *
 *  Copyright 2007
 *  National Institute of Advanced Industrial Science and Technology (AIST)
 *
 *  Author: Toshio UESHIBA
 *
 *  Confidentail and all rights reserved.
 *  This program is confidential. Any changing, copying or giving
 *  information about the source code of any part of this software
 *  and/or documents without permission by the authors are prohibited.
 *
 *  No Warranty.
 *  Authors are not responsible for any damages in the use of this program.
 *  
 *  $Id: Random.h,v 1.3 2007-11-26 07:28:09 ueshiba Exp $
 */
namespace TU
{
/************************************************************************
*  class Random								*
************************************************************************/
class Random
{
  public:
    Random()						;
    
    double	uniform()				;
    double	gaussian()				;
    double	uniform48()				;
    double	gaussian48()				;
    
  private:
    double	gaussian(double (Random::*uni)())	;
    
    int		_seed;
    long	_x1, _x2, _x3;
    double	_r[97];
    int		_ff;
    int		_has_extra;	// flag showing existence of _extra.
    double	_extra;		// extra gaussian noise value.
};

inline double
Random::gaussian()
{
    return gaussian(&Random::uniform);
}

inline double
Random::gaussian48()
{
    return gaussian(&Random::uniform48);
}
 
}
