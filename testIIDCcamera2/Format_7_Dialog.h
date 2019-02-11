/*!
 *  \file	Dialog.h
 */
#ifndef TU_QT_FORMAT_7_DIALOG_H
#define TU_QT_FORMAT_7_DIALOG_H

#include <QDialog>
#include "TU/IIDC++.h"

namespace TU
{
namespace qt
{
/************************************************************************
*  class Format_7_Dialog						*
************************************************************************/
class Format_7_Dialog : public QDialog
{
  public:
    static IIDCCamera::PixelFormat
    getParameters(QWidget* parent, const IIDCCamera::Format_7_Info& fmt7info,
		  u_int& u0, u_int& v0, u_int& width, u_int& height,
		  u_int& packetSize)					;

  private:
    Format_7_Dialog(QWidget* parent,
		    const IIDCCamera::Format_7_Info& fmt7info)		;

  private:
    IIDCCamera::Format_7_Info	_fmt7info;
};

}	// namespace qt
}	// namespace TU
#endif	// !TU_QT_FORMAT_7_DIALOG_H
