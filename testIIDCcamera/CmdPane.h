/*!
 *   \file	CmdPane.h
 */
#ifndef TU_QT_CMDPANE_H
#define TU_QT_CMDPANE_H

#include <QScrollArea>
#include <QGridLayout>
#include <QPushButton>

namespace TU
{
namespace qt
{
/************************************************************************
*  class CmdPane							*
************************************************************************/
class CmdPane : public QScrollArea
{
    Q_OBJECT

  public:
		CmdPane(QWidget* parent)				;

    template <class CAMERA>
    void	addCmds(CAMERA& camera)					;
    
  Q_SIGNALS:
    void	timerSet(bool enable)					;

  private:
    template <class CAMERA>
    void	addFormatAndFeatureCmds(CAMERA& camera)			;

  private:
    QWidget*	 const	_pane;
    QGridLayout* const	_layout;
};

inline
CmdPane::CmdPane(QWidget* parent)
    :QScrollArea(parent),
     _pane(new QWidget(this)),
     _layout(new QGridLayout(_pane))
{
    _layout->setHorizontalSpacing(4);
    _layout->setVerticalSpacing(2);
    _layout->setSizeConstraint(QLayout::SetFixedSize);
}

template <class CAMERA> void
CmdPane::addCmds(CAMERA& camera)
{
  // カメラからの画像取り込みをon/offするtoggle buttonを生成．
    const auto	toggle = new QPushButton(tr("Capture"), _pane);
    toggle->setCheckable(true);
    _pane->connect(toggle, &QPushButton::toggled,
		   [&camera, this](bool enable)
		   {
		       camera.continuousShot(enable);
		       this->timerSet(enable);
		   });
    toggle->setChecked(camera.inContinuousShot());
    _layout->addWidget(toggle, 0, 0, 1, 1);

    addFormatAndFeatureCmds(camera);

    setWidget(_pane);
}
    
}	// namespace qt
}	// namespace TU
#endif	// !TU_QT_CMDPANE_H
