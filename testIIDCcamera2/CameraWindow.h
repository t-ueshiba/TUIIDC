/*!
 *   \file	CameraWindow.h
 */
#ifndef TU_QT_CAMERAWINDOW_H
#define TU_QT_CAMERAWINDOW_H

#include <QMainWindow>
#include <QStatusBar>
#include <QTimerEvent>
#include <QElapsedTimer>
#include "TU/qt/ImageView.h"
#include "CmdPane.h"

namespace TU
{
/************************************************************************
*  global functions							*
************************************************************************/
template <class CAMERA> QString
cameraName(const CAMERA& camera)					;

namespace qt
{
/************************************************************************
*  class CameraWindow<CAMERA>						*
************************************************************************/
template <class CAMERA>
class CameraWindow : public QMainWindow
{
  public:
		CameraWindow(QWidget* parent, CAMERA& camera)		;
		~CameraWindow()						;

    void	onTimerSet(bool enable)					;

  protected:
    void	captureAndDisplay()					;
    template <class T>
    void	captureRawAndDisplay()					;
    void	captureBayerAndDisplay()				;
    void	timerEvent(QTimerEvent* event)				;

  private:
    CAMERA&		_camera;
    QVector<uchar>	_image;
    ImageView*	const	_imageView;
    CmdPane*	const	_cmdPane;
    int			_timerId;

    int			_nframes;
    QElapsedTimer	_elapsedTimer;
};

template <class CAMERA>
CameraWindow<CAMERA>::CameraWindow(QWidget* parent, CAMERA& camera)
    :QMainWindow(parent),
     _camera(camera),
     _imageView(new ImageView(this)),
     _cmdPane(new CmdPane(this)),
     _timerId(0),
     _nframes(0)
{
    setWindowTitle(cameraName(_camera));
    setAttribute(Qt::WA_DeleteOnClose);

    _cmdPane->addCmds(_camera);
    connect(_cmdPane, &CmdPane::timerSet, this, &CameraWindow::onTimerSet);

    const auto	central = new QWidget(this);
    const auto	layout  = new QHBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);
  //layout->setSizeConstraint(QLayout::SetMaximumSize);
    layout->addWidget(_imageView);
    layout->addWidget(_cmdPane);
    setCentralWidget(central);

    _elapsedTimer.start();
    show();
}

template <class CAMERA>
CameraWindow<CAMERA>::~CameraWindow()
{
    onTimerSet(false);
}

template <class CAMERA> void
CameraWindow<CAMERA>::onTimerSet(bool enable)
{
    if (enable)
    {
	if (_timerId == 0)
	    _timerId = startTimer(2);	// interval = 2ms (500Hz)
    }
    else
    {
	if (_timerId != 0)
	{
	    killTimer(_timerId);
	    _timerId = 0;
	}
    }
}

template <class CAMERA> template <class T> inline void
CameraWindow<CAMERA>::captureRawAndDisplay()
{
    _image.resize(_camera.width() * _camera.height() * sizeof(T));
    _camera.snap().captureRaw(_image.data());
    _imageView->display(reinterpret_cast<const T*>(_image.data()),
			_camera.width(), _camera.height());
}

template <class CAMERA> inline void
CameraWindow<CAMERA>::captureBayerAndDisplay()
{
    _image.resize(_camera.width() * _camera.height() * sizeof(RGB));
    _camera.snap().captureBayerRaw(_image.data());
    _imageView->display(reinterpret_cast<const RGB*>(_image.data()),
			_camera.width(), _camera.height());
}

template <class CAMERA> void
CameraWindow<CAMERA>::timerEvent(QTimerEvent* event)
{
    if (event->timerId() != _timerId)
	return;

    captureAndDisplay();

    if (_nframes++ == 10)
    {
	const auto	elapsed = _elapsedTimer.elapsed()*1e-3;
	statusBar()->showMessage(QString::number(_nframes/elapsed)
				 .append("fps"));
	_nframes = 0;
	_elapsedTimer.restart();
    }
}

}	// namespace qt
}	// namespace TU
#endif	// !TU_QT_CAMERAWINDOW_H
