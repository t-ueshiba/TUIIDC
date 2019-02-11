/*!
 *  \file	Format_7_Dialog.cc
 */
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QMenu>
#include <QAction>
#include <QDialogButtonBox>
#include "TU/qt/Slider.h"
#include "Format_7_Dialog.h"

namespace TU
{
namespace qt
{
/************************************************************************
*  class Format_7_Dialog						*
************************************************************************/
Format_7_Dialog::Format_7_Dialog(QWidget* parent,
				 const IIDCCamera::Format_7_Info& fmt7info)
    :QDialog(parent), _fmt7info(fmt7info)
{
    int		row = 0;
    auto	layout = new QGridLayout(this);
    layout->setHorizontalSpacing(4);
    layout->setVerticalSpacing(2);

    auto	label = new QLabel(tr("u0"), this);
    label->setAlignment(Qt::Alignment(Qt::AlignRight | Qt::AlignVCenter));
    const auto	u0 = new Slider(this);
    u0->setRange(0, _fmt7info.maxWidth, _fmt7info.unitU0);
    u0->setValue(_fmt7info.u0);
    layout->addWidget(label, row, 0, 1, 1);
    layout->addWidget(u0,    row, 1, 1, 1);
    ++row;

    label = new QLabel(tr("v0"), this);
    label->setAlignment(Qt::Alignment(Qt::AlignRight | Qt::AlignVCenter));
    const auto	v0 = new Slider(this);
    v0->setRange(0, _fmt7info.maxHeight, _fmt7info.unitV0);
    v0->setValue(_fmt7info.v0);
    layout->addWidget(label, row, 0, 1, 1);
    layout->addWidget(v0,    row, 1, 1, 1);
    ++row;

    label = new QLabel(tr("width"), this);
    label->setAlignment(Qt::Alignment(Qt::AlignRight | Qt::AlignVCenter));
    const auto	width = new Slider(this);
    width->setRange(0, _fmt7info.maxWidth, _fmt7info.unitWidth);
    width->setValue(_fmt7info.width);
    layout->addWidget(label, row, 0, 1, 1);
    layout->addWidget(width, row, 1, 1, 1);
    ++row;

    label = new QLabel(tr("height"), this);
    label->setAlignment(Qt::Alignment(Qt::AlignRight | Qt::AlignVCenter));
    const auto	height = new Slider(this);
    height->setRange(0, _fmt7info.maxHeight, _fmt7info.unitHeight);
    height->setValue(_fmt7info.height);
    layout->addWidget(label,  row, 0, 1, 1);
    layout->addWidget(height, row, 1, 1, 1);
    ++row;

    label = new QLabel(tr("packet size"), this);
    label->setAlignment(Qt::Alignment(Qt::AlignRight | Qt::AlignVCenter));
    const auto	bytePerPacket = new Slider(this);
    bytePerPacket->setRange(_fmt7info.unitBytePerPacket,
			    _fmt7info.maxBytePerPacket,
			    _fmt7info.unitBytePerPacket);
    bytePerPacket->setValue(_fmt7info.bytePerPacket);
    layout->addWidget(label,	     row, 0, 1, 1);
    layout->addWidget(bytePerPacket, row, 1, 1, 1);
    ++row;

    connect(u0, &Slider::valueChanged,
	    [this, width](double val)
	    {
		_fmt7info.u0 = u_int(val);
		if (_fmt7info.u0 + _fmt7info.width > _fmt7info.maxWidth)
		{
		    _fmt7info.width = _fmt7info.maxWidth - _fmt7info.u0;
		    width->setValue(_fmt7info.width);
		}
	    });
    connect(v0, &Slider::valueChanged,
	    [this, height](double val)
	    {
		_fmt7info.v0 = u_int(val);
		if (_fmt7info.v0 + _fmt7info.height > _fmt7info.maxHeight)
		{
		    _fmt7info.height = _fmt7info.maxHeight - _fmt7info.v0;
		    height->setValue(_fmt7info.height);
		}
	    });
    connect(width, &Slider::valueChanged,
	    [this, u0](double val)
	    {
		_fmt7info.width = u_int(val);
		if (_fmt7info.u0 + _fmt7info.width > _fmt7info.maxWidth)
		{
		    _fmt7info.u0 = 0;
		    u0->setValue(_fmt7info.u0);
		}
	    });
    connect(height, &Slider::valueChanged,
	    [this, v0](double val)
	    {
		_fmt7info.height = u_int(val);
		if (_fmt7info.v0 + _fmt7info.height > _fmt7info.maxHeight)
		{
		    _fmt7info.v0 = 0;
		    v0->setValue(_fmt7info.v0);
		}
	    });
    connect(bytePerPacket, &Slider::valueChanged,
	    [this](double val){ _fmt7info.bytePerPacket = u_int(val); });

    label = new QLabel(tr("pixel format"), this);
    const auto	button = new QPushButton(this);
    const auto	menu   = new QMenu(button);
    for (const auto& pixelFormatName : IIDCCamera::pixelFormatNames)
    {
	const auto	pixelFormat = pixelFormatName.pixelFormat;
	const auto	name	    = pixelFormatName.name;

	if (_fmt7info.availablePixelFormats & pixelFormat)
	{
	    const auto	action = new QAction(tr(name), menu);
	    menu->addAction(action);
	    connect(action, &QAction::triggered,
		    [this, pixelFormat, button, name]()
		    {
			_fmt7info.pixelFormat = pixelFormat;
			button->setText(tr(name));
		    });

	    if (_fmt7info.pixelFormat == pixelFormat)
		button->setText(tr(name));
	}
    }
    button->setMenu(menu);
    layout->addWidget(label,  row, 0, 1, 1);
    layout->addWidget(button, row, 1, 1, 1);
    ++row;

    const auto	dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok,
						       Qt::Horizontal, this);
    layout->addWidget(dialogButtonBox, row, 0, 1, 2);

    connect(dialogButtonBox, &QDialogButtonBox::accepted,
	    this, &QDialog::accept);

    exec();
}

IIDCCamera::PixelFormat
Format_7_Dialog::getParameters(QWidget* parent,
			       const IIDCCamera::Format_7_Info& fmt7info,
			       u_int& u0,    u_int& v0,
			       u_int& width, u_int& height,
			       u_int& bytePerPacket)
{
    Format_7_Dialog	dialog(parent, fmt7info);

    u0		  = dialog._fmt7info.u0;
    v0		  = dialog._fmt7info.v0;
    width	  = dialog._fmt7info.width;
    height	  = dialog._fmt7info.height;
    bytePerPacket = dialog._fmt7info.bytePerPacket;

    return dialog._fmt7info.pixelFormat;
}

}	// namespace qt
}	// namespace TU
