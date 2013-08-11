// =====================================================================================
// 
//       Filename:  main.cpp
//
//    Description:  demo 的 main 函数
//
//        Version:  1.0
//        Created:  2013年06月16日 15时02分17秒
//       Revision:  none
//       Compiler:  g++
//
//         Author:  Hurley (LiuHuan), liuhuan1992@gmail.com
//        Company:  Class 1107 of Computer Science and Technology
// 
// =====================================================================================

#include <QApplication>
#include <QtGui>

#include "MainWidget.h"

int main(int argc, char *argv[])
{
	// 处理中文乱码问题
	QTextCodec *codec = QTextCodec::codecForName("system");

	QTextCodec::setCodecForTr(codec);
	QTextCodec::setCodecForLocale(codec);
	QTextCodec::setCodecForCStrings(codec);

	QApplication app(argc, argv);
	MainWidget *widget = new MainWidget();
/*
#ifndef WIN32
	// 去掉标题栏
	widget->setWindowFlags(Qt::X11BypassWindowManagerHint);  
#endif
*/
	// 窗口移动到屏幕中间并显示
	widget->move((QApplication::desktop()->width() - widget->width()) >> 1,
				(QApplication::desktop()->height() - widget->height()) >> 1);
	widget->show();


	return app.exec();
}

