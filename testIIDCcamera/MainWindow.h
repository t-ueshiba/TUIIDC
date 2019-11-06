/*!
 *   \file	MainWindow.h
 */
#ifndef TU_QT_MAINWINDOW_H
#define TU_QT_MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <yaml-cpp/yaml.h>
#include <fstream>
#include "CameraWindow.h"

namespace TU
{
namespace qt
{
/************************************************************************
*  local data                                                           *
************************************************************************/
/*!
 *  \brief	上矢印のアイコン画像．
 */
static const char* up_xpm[] =
{
    "16 16 20 1",
    "       c None",
    ".      c #C0C0C0",
    "+      c #E0E0E0",
    "@      c #949494",
    "#      c #D3D3D3",
    "$      c #C8C8C8",
    "%      c #A1A1A1",
    "&      c #F7F7F7",
    "*      c #D4D4D4",
    "=      c #E5E5E5",
    "-      c #F1F1F1",
    ";      c #BBBBBB",
    ">      c #CECECE",
    ",      c #F0F0F0",
    "'      c #AEAEAE",
    ")      c #DADADA",
    "!      c #F6F6F6",
    "~      c #EAEAEA",
    "{      c #EBEBEB",
    "]      c #ECECEC",
    "                ",
    "       .+       ",
    "      @#$       ",
    "      %$&$      ",
    "     %$*=-$     ",
    "    @;>++,$     ",
    "    '$)+++!$    ",
    "   %$*++++=-$   ",
    "  @;>++++++,$   ",
    "  '$)+++++++!$  ",
    " %$*++++++++=-$ ",
    "@;$***+++=~~~!$ ",
    "@''''$+++{]'''* ",
    "    @$+++]'     ",
    "    @;$$$+'     ",
    "     @@@@@@     "
};

/*!
 *  \brief	下矢印のアイコン画像．
 */
static const char* down_xpm[] =
{
    "16 16 20 1",
    "       c None",
    ".      c #949494",
    "+      c #AEAEAE",
    "@      c #E0E0E0",
    "#      c #C8C8C8",
    "$      c #BBBBBB",
    "%      c #ECECEC",
    "&      c #D4D4D4",
    "*      c #EBEBEB",
    "=      c #F6F6F6",
    "-      c #EAEAEA",
    ";      c #E5E5E5",
    ">      c #F1F1F1",
    ",      c #A1A1A1",
    "'      c #DADADA",
    ")      c #F0F0F0",
    "!      c #CECECE",
    "~      c #F7F7F7",
    "{      c #D3D3D3",
    "]      c #C0C0C0",
    "     ......     ",
    "     +@###$.    ",
    "     +%@@@#.    ",
    " &+++%*@@@#++++.",
    " #=---;@@@&&&#$.",
    " #>;@@@@@@@@&#, ",
    "  #=@@@@@@@'#+  ",
    "   #)@@@@@@!$.  ",
    "   #>;@@@@&#,   ",
    "    #=@@@'#+    ",
    "     #)@@!$.    ",
    "     #>;&#,     ",
    "      #~#,      ",
    "       #{.      ",
    "       @]       ",
    "                "
};

/*!
 *  \brief	終了のアイコン画像．
 */
static const char* exit_xpm[] =
{
    "32 32 11 1",
    " 	c None",
    ".	c #000000",
    "+	c #323232",
    "@	c #4C4C4C",
    "#	c #808080",
    "$	c #C48B1B",
    "%	c #DE9D1E",
    "&	c #AB7917",
    "*	c #FFFFFF",
    "=	c #E6E6E6",
    "-	c #B2B2B2",
    "....................            ",
    ".+.+.+.+.+.+...+.+..            ",
    "..@#@@#@@#@@#@@#@@+.            ",
    ".+$$#@@#@@#@@#@@#@..            ",
    "..%&$&@@#@@#@@#@@#***=          ",
    ".+$%$$&&#@#@#@#@#*****=         ",
    "..%$%&$$&&@#@#@#@****==         ",
    ".+$%$%%&$&&@#@@##****=-         ",
    "..%%%$%$&$&####@@#*==-#         ",
    ".+%$%%$%$$&@@###@@@--#@--       ",
    "..%%%$%%$&$##@#@##+.*****=--    ",
    ".+%%%%%$$$&#@#@##@.*****=**=-   ",
    "..%%%%$%$&$####@##*****=-=**=-  ",
    ".+%%%%%%$$&##@#-#**=***=- -=*=- ",
    "..%%%%%$$$&@#####**==**=-  -**# ",
    ".+%%%%%%$$$#-*****=.=**=-   -=# ",
    "..%%%%%%$$$@#=====+.-**==-  -## ",
    ".+%%%%%%%$&##-##-#..-****-      ",
    "..%%%%%%@+&#######+.-****-      ",
    ".+%%%%%%%.$@-##-#@..-****-      ",
    "..%%%%%%$$$##-##-#+.***=**-     ",
    ".+%%%%%%$&&####-##..***==*-     ",
    "..%%%%%%$&$-#-###-+.***@=*-     ",
    ".+%%%%%$$$&#-#-#-#.***@ =*--    ",
    "..%%%$%&&$&-#-#-#-+**@  @=**--- ",
    ".+%$%%$%$&&#-##-##**=@   @=****-",
    "..%%$&%$&$&-#-#-#-**@     =====@",
    ".+$%%$$&$&&#-#-#-**=@     -@@@@ ",
    "..%&%$&&&&#-----***@            ",
    ".+$$&&&&-#-#--#=**-.#           ",
    "..+.+.+.+.+.+.+.+.+.@####@@@@@# ",
    "....................##- -####-  "
};

/*!
 *  \brief	保存のアイコン画像．
 */
static const char* save_xpm[] =
{
    "32 32 24 1",
    " 	c None",
    ".	c #8A86C8",
    "+	c #CBCBCB",
    "@	c #666666",
    "#	c #999999",
    "$	c #E0E0E0",
    "%	c #000000",
    "&	c #FFFFFF",
    "*	c #7974C8",
    "=	c #B0ADE0",
    "-	c #333333",
    ";	c #100000",
    ">	c #655EC8",
    ",	c #AAAAAA",
    "'	c #6D68C8",
    ")	c #4840AD",
    "!	c #281EAD",
    "~	c #BEBEBE",
    "{	c #5B53BA",
    "]	c #1C157A",
    "^	c #D26F6F",
    "/	c #EBABAB",
    "(	c #D24646",
    "_	c #949494",
    "                                ",
    "                    ..          ",
    "                 +@#$..         ",
    "            #+@%@$&&#*==.       ",
    "        ##+#$+-;-+$&#>*==.      ",
    "    >@#,+$$$$$#;##$$&#'*=*.     ",
    " >)!>@#~+$+$+$+-#-+$$#.'*=.     ",
    "!.>'{),~,+$$$$+-;##+$@>>.*..    ",
    "!{>.{!##~++$+$$#-#-++$#>>'=.    ",
    "!)>'>)@#+,+~$$$+---#+$@.>*.*    ",
    "])>'.'-,#+~++++$#-++$$~#>'=>    ",
    ">!{>'{)#~#,+,+~+$+$~#~@#>**..   ",
    ".!)''.!@#~~~~+,~#~##@#.>>>>=.   ",
    " ]!>.')-,##~####@@@.{>>>>>*.*   ",
    " ]!{''{!##,#@@.{{.{>.>{.{{*.>   ",
    " >!)>>{)!-@{{{{>>>>>{{{@#~{*..  ",
    " .!){>>{{{{>>>>{{{{{.@$&&&#{..  ",
    "  ]!)>>>>>>>{{{.#@~$&&&&^^@{=*  ",
    "  ]!)>>{{{.#@@#$&&&&/^^((^@{>.  ",
    "  >!){{##@$&&&&&/^^((((^^/&#{.. ",
    "  .]){~$&&&//^^((((^^//&&&_@{.. ",
    "   !!{#&/^^((((^^//&&&&$~$&]{>. ",
    "   ]!)@^^((^^/&&&&&$_~$&$$@#{>> ",
    "   >]){@^^/&&&$~_~$&&$$@#..>>){ ",
    "   .!!{@&&&$~_&&&$$@@##.>>{{){{ ",
    "    ]!)#$~$&&$$@@##.>>>{{)!]!)  ",
    "    ]!){]$@@@#..>>>{))!!])>     ",
    "    >!!{#]>{){){))!!!]>>        ",
    "    .!!)){{))!!!!]]>            ",
    "     ]!!!!]!]!]>.               ",
    "     ]!]!]>..                   ",
    "     ]]>.                       "
};

/*!
 *  \brief	カメラのアイコン画像．
 */
static const char* camera_xpm[] =
{
    "32 32 43 1",
    " 	c None",
    ".	c #C8EAC7",
    "+	c #B2D7B1",
    "@	c #79C678",
    "#	c #5CB15A",
    "$	c #4BD94B",
    "%	c #5FF85F",
    "&	c #2ECD2E",
    "*	c #83A782",
    "=	c #739472",
    "-	c #699B69",
    ";	c #12B00E",
    ">	c #6A6680",
    ",	c #898999",
    "'	c #4E4B5E",
    ")	c #B9B8CC",
    "!	c #319631",
    "~	c #737380",
    "{	c #22A022",
    "]	c #2E2E33",
    "^	c #A2A1B2",
    "/	c #D0CFE6",
    "(	c #A1A1B2",
    "_	c #658264",
    ":	c #000000",
    "<	c #7C7A99",
    "[	c #B6C6B6",
    "}	c #8A8A99",
    "|	c #E6E6FF",
    "1	c #226820",
    "2	c #4D534D",
    "3	c #427041",
    "4	c #333B33",
    "5	c #B8B8CC",
    "6	c #758775",
    "7	c #FFB7D4",
    "8	c #C83972",
    "9	c #750B0B",
    "0	c #A3B0A3",
    "a	c #C9D2C9",
    "b	c #849384",
    "c	c #BEBEBE",
    "d	c #779477",
    "                         .+@+.  ",
    "                 .++@#$@$$%%$+  ",
    "        ..++@##@##&$$*==*-&$%$  ",
    "   .++@#@+#;&&;**===*>>,,*;&$%. ",
    "+@#++;&&;**==**>'''>>,,)),*;&$+ ",
    "!;;*===**>'''>>,~~,,,~~,)>,!&$$ ",
    "{!=']>'>>,,,,,~~,^^////~~,>*$&$ ",
    "&=']]',,(,(,,~>>^^)))))//~>=;$;.",
    "{_]:]>,)(,,,~>)<<)>''''))/'*-[[@",
    "!_':]'>(,,,~},'')>>>']]'')|*+122",
    "33_':]',,,,~>)'])>:::~~]')/(*24[",
    "!1_]:]>(,,~~,}])>::::55~]')/*@[.",
    "13_]:]'>,,~})]])>:::5|5~]')/*36;",
    "33_'::]',~'>}]:)>:7:|5::>')/*&;&",
    "1!__':]'~~'>>::)>:89::::>>)/*=$;",
    "!3!_]:]>,~']}:::)>987:]5>>)^,*;;",
    "!3!_]:]'>~'>}:::)>:::]~]>)^^>*=&",
    ";1!_'::]'~''}}::])>::]]>>)<'',*!",
    "{!3;_]:]'~~'>}:]:])>>>>')<^'~>*;",
    "!1!;_]:]>,~':>}]]:])))))'<)~>'*{",
    "3!=&_]:]'>~'']>}]':]]]]')),~,'*=",
    ";!;_3'::]>,~''>>}]''~}((),~,(>}*",
    "0;&;;_':]',~~'']>}'~}(()(~,(,'>*",
    "0!{;&_]:]'~,~~'']]^^~~~~~~']]]'*",
    " ;@a[_]:]>,~,~~~''~'']]]'']]:]'*",
    " b)24a':]]',~,']]]'']]:]]]':]'=;",
    " +422@_]]''''']]:]]]']]:]'__=={{",
    "  #++*_]']]]]]']]:]'____==&;{60b",
    "  $*c&$_]]::]'____==&;6db313b0  ",
    "  bc!;33==__==&;d3313b0         ",
    "  0&;1;6b3;1;;b0                ",
    "   33330                        "
};

/************************************************************************
*  class MainWindow<CAMERA>						*
************************************************************************/
template <class CAMERA>
class MainWindow : public QMainWindow
{
  public:
			MainWindow()					;

  private:
    void		addExtraCmds()					{}
    void		up()						;
    void		down()						;
    void		save()						;

    static CAMERA	createCamera(int n)				;
    static QString	defaultConfigFile()				;

  private:
    std::vector<CAMERA>		_cameras;
    QWidget*		const	_central;
    QGridLayout*	const	_layout;
    QListWidget*	const	_list;
};

template <class CAMERA>
MainWindow<CAMERA>::MainWindow()
    :QMainWindow(),
     _central(new QWidget(this)),
     _layout(new QGridLayout(_central)),
     _list(new QListWidget(_central))
{
    _layout->setHorizontalSpacing(2);
    _layout->setVerticalSpacing(2);

  // 利用可能な全てのカメラを列挙してリストに入れる．
    for (int n = 0; ; ++n)
    {
	try
	{
	    _cameras.emplace_back(createCamera(n));
	    _list->addItem(new QListWidgetItem(QPixmap(camera_xpm),
					       cameraName(_cameras.back()),
					       _list));
	}
	catch (const std::exception& err)
	{
	    break;
	}
    }

    if (_list->count() > 0)
	_list->setCurrentRow(0);

  // リスト中のカメラが選択されたらそれを操作するウィンドウを生成し，
  // リストから隠す．ウィンドウが破壊されたら再びリストに表示する．
    connect(_list, &QListWidget::itemDoubleClicked,
	    [this](QListWidgetItem* item)
	    {
		const auto
		    cameraWindow = new CameraWindow<CAMERA>(
					  this, _cameras[_list->row(item)]);
		connect(cameraWindow, &QObject::destroyed,
			[item](){ item->setHidden(false); });
		item->setHidden(true);
	    });
    _layout->addWidget(_list, 0, 0, 2, 1);

  // ボタンを生成する．
    auto	button = new QPushButton(QPixmap(up_xpm), tr(""), _central);
    button->setSizePolicy(QSizePolicy::Preferred,
			  QSizePolicy::MinimumExpanding);
    connect(button, &QPushButton::clicked, this, &MainWindow::up);
    _layout->addWidget(button, 0, 1, 1, 1);

    button = new QPushButton(QPixmap(down_xpm), tr(""), _central);
    button->setSizePolicy(QSizePolicy::Preferred,
			  QSizePolicy::MinimumExpanding);
    connect(button, &QPushButton::clicked, this, &MainWindow::down);
    _layout->addWidget(button, 1, 1, 1, 1);

    button = new QPushButton(QPixmap(save_xpm), tr("Save"), _central);
    connect(button, &QPushButton::clicked, this, &MainWindow::save);
    _layout->addWidget(button, 0, 2, 1, 1);

    button = new QPushButton(QPixmap(exit_xpm), tr("Exit"), _central);
    connect(button, &QPushButton::clicked, this, &QMainWindow::close);
    _layout->addWidget(button, 1, 2, 1, 1);

    addExtraCmds();

    setCentralWidget(_central);

    show();
}

template <class CAMERA> void
MainWindow<CAMERA>::up()
{
    const auto	row = _list->currentRow();

    for (auto i = row; i < _list->count(); ++i)
	if (!_list->item(i)->isHidden())
	{
	    for (auto j = row; --j >= 0; )
		if (!_list->item(j)->isHidden())
		{
		    std::swap(*_list->item(i), *_list->item(j));
		    std::swap(_cameras[i], _cameras[j]);
		    _list->setCurrentRow(j);

		    break;
		}

	    break;
	}
}

template <class CAMERA> void
MainWindow<CAMERA>::down()
{
    const auto	row = _list->currentRow();

    for (auto i = row; i >= 0; --i)
	if (!_list->item(i)->isHidden())
	{
	    for (auto j = row; ++j < _list->count(); )
		if (!_list->item(j)->isHidden())
		{
		    std::swap(*_list->item(i), *_list->item(j));
		    std::swap(_cameras[i], _cameras[j]);
		    _list->setCurrentRow(j);

		    break;
		}

	    break;
	}
}

template <class CAMERA> void
MainWindow<CAMERA>::save()
{
    if (_cameras.size() == 0)
	return;

    const auto	fileName = QFileDialog::getSaveFileName(
				_central, tr("Save config."),
				defaultConfigFile(),
				tr("camera_name (*.conf)"));
    if (fileName.isEmpty())
	return;

    std::ofstream	out(fileName.toUtf8().data());
    if (!out)
    {
	QMessageBox::critical(this, tr("Error"), "Cannot open " + fileName);
	return;
    }

    YAML::Emitter	emitter;
    emitter << YAML::BeginSeq;
    for (const auto& camera : _cameras)
	emitter << camera;
    emitter << YAML::EndSeq;

    out << emitter.c_str() << std::endl;
}

}	// namespace qt
}	// namespace TU
#endif	// !TU_QT_MAINWINDOW_H
