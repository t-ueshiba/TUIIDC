/*!
 *  \file	IIDCSpecializations.cc
 */
#include <QLabel>
#include <QMenu>
#include "TU/IIDCCameraArray.h"
#include "TU/qt/Slider.h"
#include "MainWindow.h"
#include "Format_7_Dialog.h"

#ifndef TUIIDCPP_CONF_DIR
#  define TUIIDCPP_CONF_DIR	"/usr/local/etc"
#endif

namespace TU
{
/************************************************************************
*  global functions							*
************************************************************************/
template <> QString
cameraName(const IIDCCamera& camera)
{
    return QString::number(camera.globalUniqueId(), 16).prepend("0x");
}

namespace qt
{
/************************************************************************
*  class MainWindow<IIDCCamera>						*
************************************************************************/
template <> IIDCCamera
MainWindow<IIDCCamera>::createCamera(int n)
{
    return IIDCCamera(0);
}

template <> QString
MainWindow<IIDCCamera>::defaultConfigFile()
{
    return tr(TUIIDCPP_CONF_DIR).append("/")
	  .append(IIDCCameraArray::DEFAULT_CAMERA_NAME).append(".conf");
}

template <> void
MainWindow<IIDCCamera>::addExtraCmds()
{
    const auto	button = new QPushButton(tr("Speed"), _central);
    const auto	menu   = new QMenu(button);

    for (const auto& speedName : IIDCCamera::speedNames)
    {
	const auto	name   = speedName.name;
	const auto	speed  = speedName.speed;
	const auto	action = new QAction(tr(name), menu);
	menu->addAction(action);
	connect(action, &QAction::triggered,
		[this, speed, button, name]()
		{
		    try
		    {
			for (auto&& camera : _cameras)
			    camera.setSpeed(speed);
			button->setText(tr(name));
		    }
		    catch (const std::exception& err)
		    {
			QMessageBox::critical(this,
					      tr("Error"), tr(err.what()));
		    }
		});

	if (_cameras.size() > 0 && _cameras[0].getSpeed() == speed)
	    button->setText(tr(name));
    }

    button->setMenu(menu);
    _layout->addWidget(button, 2, 0, 1, 3);
}

/************************************************************************
*  class CameraWindow<IIDCCamera>					*
************************************************************************/
template <> void
CameraWindow<IIDCCamera>::captureAndDisplay()
{
    switch (_camera.pixelFormat())
    {
      case IIDCCamera::MONO_8:
	if (_camera.bayerTileMapping() != IIDCCamera::YYYY)
	    captureBayerAndDisplay();
	else
	    captureRawAndDisplay<uint8_t>();
	break;

      case IIDCCamera::RAW_8:
	captureRawAndDisplay<uint8_t>();
	break;

      case IIDCCamera::YUV_411:
	captureRawAndDisplay<YUV411>();
	break;

      case IIDCCamera::MONO_16:
	if (_camera.bayerTileMapping() != IIDCCamera::YYYY)
	    captureBayerAndDisplay();
	else
	    captureRawAndDisplay<uint16_t>();
	break;

      case IIDCCamera::RAW_16:
	captureRawAndDisplay<uint16_t>();
	break;

      case IIDCCamera::SIGNED_MONO_16:
	captureRawAndDisplay<int16_t>();
	break;

      case IIDCCamera::YUV_422:
	captureRawAndDisplay<YUV422>();
	break;

      case IIDCCamera::YUV_444:
	captureRawAndDisplay<YUV444>();
	break;

      case IIDCCamera::RGB_24:
	captureRawAndDisplay<RGB>();
	break;

      default:
	break;
    }
}

/************************************************************************
*  static functions							*
************************************************************************/
static void
resetSliders(const IIDCCamera& camera, IIDCCamera::Feature feature,
	     Slider* slider, Slider* slider2=nullptr)
{
    if (camera.isAbsControl(feature))
    {
	float		min, max;
	camera.getMinMax(feature, min, max);
	const auto	step = (max - min)/100;
	slider->setRange(min, max, step);

	if (feature == IIDCCamera::WHITE_BALANCE)
	{
	    slider2->setRange(min, max, step);

	    float	ub, vr;
	    camera.getWhiteBalance(ub, vr);
	    slider->setValue(ub);
	    slider2->setValue(vr);
	}
	else
	    slider->setValue(camera.getValue<float>(feature));
    }
    else
    {
	u_int	min, max;
	camera.getMinMax(feature, min, max);
	slider->setRange(min, max, 1);

	if (feature == IIDCCamera::WHITE_BALANCE)
	{
	    slider2->setRange(min, max, 1);

	    u_int	ub, vr;
	    camera.getWhiteBalance(ub, vr);
	    slider->setValue(ub);
	    slider2->setValue(vr);
	}
	else
	    slider->setValue(camera.getValue<u_int>(feature));
    }
}

/************************************************************************
*  class CmdPane							*
************************************************************************/
template <> void
CmdPane::addFormatAndFeatureCmds(IIDCCamera& camera)
{
    const auto	button = new QPushButton(_pane);
    const auto	menu   = new QMenu(button);

    for (const auto& formatName : IIDCCamera::formatNames)
    {
	const auto	name   = formatName.name;
	const auto	format = formatName.format;
	const auto	inq    = camera.inquireFrameRate(format);
	QMenu*		frameRateMenu = nullptr;

	for (const auto& frameRateName : IIDCCamera::frameRateNames)
	{
	    const auto	frameRate = frameRateName.frameRate;

	    if (inq & frameRate)
	    {
		if (!frameRateMenu)
		{
		    frameRateMenu = new QMenu(menu);

		    const auto
			formatAction = new QAction(tr(name), menu);
		    formatAction->setMenu(frameRateMenu);
		    menu->addAction(formatAction);
		}

		const auto
		    frameRateAction = new QAction(tr(frameRateName.name),
						  frameRateMenu);
		frameRateMenu->addAction(frameRateAction);
		connect(frameRateAction, &QAction::triggered,
			[&camera, format, frameRate, button, name]()
			{
			    try
			    {
				if (format >= IIDCCamera::Format_7_0)
				{
				    u_int	u0, v0,
						width, height, bytePerPacket;
				    const auto	pixelFormat
					= Format_7_Dialog::getParameters(
					    camera.getFormat_7_Info(format),
					    u0, v0, width, height,
					    bytePerPacket);
				    camera.setFormat_7_ROI(format, u0, v0,
							   width, height)
					.setFormat_7_PixelFormat(format,
								 pixelFormat)
					.setFormat_7_PacketSize(format,
								bytePerPacket);
				}
				camera.setFormatAndFrameRate(format, frameRate);

				button->setText(tr(name));
			    }
			    catch (const std::exception& err)
			    {
				QMessageBox::critical(nullptr, tr("Error"),
						      tr(err.what()));
			    }
			});
	    }
	}

	if (camera.getFormat() == format)
	    button->setText(tr(name));

    }
    button->setMenu(menu);
    _layout->addWidget(button, 0, 1, 1, 1);

    auto	row = _layout->rowCount();

    for (const auto& featureName : IIDCCamera::featureNames)
    {
	const auto	feature = featureName.feature;
	const auto	inq = camera.inquireFeatureFunction(feature);

	if (!((inq & IIDCCamera::Presence) &&
	      (inq & IIDCCamera::Manual)   &&
	      (inq & IIDCCamera::ReadOut)))
	    continue;

	const auto	label = new QLabel(tr(featureName.name), _pane);
	label->setAlignment(Qt::Alignment(Qt::AlignRight | Qt::AlignVCenter));
	_layout->addWidget(label, row, 0, 1, 1);

	Slider*	slider  = nullptr;
	Slider*	slider2 = nullptr;

	switch (feature)
	{
	  case IIDCCamera::TRIGGER_MODE:
	  {
	  // カメラのtrigger modeをon/offするtoggle buttonを生成．
	    const auto	button = new QPushButton(_pane);
	    const auto	menu   = new QMenu(button);

	    for (const auto& triggerModeName : IIDCCamera::triggerModeNames)
	    {
		const auto	name	    = triggerModeName.name;
		const auto	triggerMode = triggerModeName.triggerMode;

		if (inq & triggerMode)
		{
		    const auto	action = new QAction(tr(name), menu);
		    menu->addAction(action);

		    connect(action, &QAction::triggered,
			    [&camera, triggerMode, button, name]()
			    {
				camera.setTriggerMode(triggerMode);
				button->setText(tr(name));
			    });

		    if (camera.getTriggerMode() == triggerMode)
			button->setText(tr(name));
		}
	    }

	    button->setMenu(menu);
	    _layout->addWidget(button, row, 1, 1, 1);
	  }
	    break;

	  case IIDCCamera::WHITE_BALANCE:
	  {
	    slider = new Slider(_pane);
	    connect(slider, &Slider::valueChanged,
		    [&camera, feature](double val)
		    {
			if (camera.isAbsControl(feature))
			{
			    float	ub, vr;
			    camera.getWhiteBalance(ub, vr);
			    camera.setWhiteBalance(val, vr);
			}
			else
			{
			    u_int	ub, vr;
			    camera.getWhiteBalance(ub, vr);
			    camera.setWhiteBalance(val, vr);
			}
		    });
	    _layout->addWidget(slider, row, 1, 1, 1);

	    const auto	label2 = new QLabel(tr("White bal.(V/R)"), _pane);
	    label2->setAlignment(Qt::Alignment(Qt::AlignRight |
					       Qt::AlignVCenter));
	    _layout->addWidget(label2, row + 1, 0, 1, 1);
	    slider2 = new Slider(_pane);
	    connect(slider2, &Slider::valueChanged,
		    [&camera, feature](double val)
		    {
			if (camera.isAbsControl(feature))
			{
			    float	ub, vr;
			    camera.getWhiteBalance(ub, vr);
			    camera.setWhiteBalance(ub, val);
			}
			else
			{
			    u_int	ub, vr;
			    camera.getWhiteBalance(ub, vr);
			    camera.setWhiteBalance(ub, val);
			}
		    });
	    resetSliders(camera, feature, slider, slider2);
	    _layout->addWidget(slider2, row + 1, 1, 1, 1);
	  }
	    break;

	  default:
	    slider = new Slider(_pane);
	    connect(slider, &Slider::valueChanged,
		    [&camera, feature](double val)
		    {
			if (camera.isAbsControl(feature))
			    camera.setValue(feature, float(val));
			else
			    camera.setValue(feature, u_int(val));
		    });
	    resetSliders(camera, feature, slider);
	    _layout->addWidget(slider, row, 1, 1, 1);
	    break;
	}

	if (inq & IIDCCamera::OnOff)		// on/off操作が可能？
	{
	    const auto	toggle = new QPushButton(tr("On"), _pane);
	    toggle->setCheckable(true);
	    connect(toggle, &QPushButton::toggled,
		    [&camera, feature](bool enable)
		    { camera.setActive(feature, enable); });
	    toggle->setChecked(camera.isActive(feature));
	    toggle->setMaximumWidth(40);
	    _layout->addWidget(toggle, row, 2, 1, 1);
	}

	if (inq & IIDCCamera::Auto)		// 自動設定が可能？
	{
	    if (feature == IIDCCamera::TRIGGER_MODE)
	    {
		const auto	toggle = new QPushButton(tr("(+)"), _pane);
		toggle->setCheckable(true);
		connect(toggle, &QPushButton::toggled,
			[&camera](bool highActive)
			{ camera.setTriggerPolarity(highActive); });
		toggle->setChecked(camera.getTriggerPolarity());
		toggle->setMaximumWidth(40);
		_layout->addWidget(toggle, row, 3, 1, 1);
	    }
	    else
	    {
		const auto	toggle = new QPushButton(tr("Auto"), _pane);
		toggle->setCheckable(true);
		connect(toggle, &QPushButton::toggled,
			[&camera, feature](bool enable)
			{ camera.setAuto(feature, enable); });
		toggle->setChecked(camera.isAuto(feature));
		toggle->setMaximumWidth(40);
		_layout->addWidget(toggle, row, 3, 1, 1);
	    }
	}

	if (inq & IIDCCamera::Abs_Control)	// 絶対値での操作が可能？
	{
	    const auto	toggle = new QPushButton(tr("Abs"), _pane);
	    toggle->setCheckable(true);
	    connect(toggle, &QPushButton::toggled,
		    [&camera, feature, slider, slider2](bool enable)
		    {
			camera.setAbsControl(feature, enable);
			resetSliders(camera, feature, slider, slider2);
		    });
	    toggle->setChecked(camera.isAbsControl(feature));
	    toggle->setMaximumWidth(40);
	    _layout->addWidget(toggle, row, 4, 1, 1);
	}

	if (slider2)
	    row += 2;
	else
	    ++row;
    }
}

}	// namespace qt
}	// namespace TU
