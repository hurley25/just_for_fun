// =====================================================================================
// 
//       Filename:  main.cpp
//
//    Description:  创建一个简单窗口显示摄像头数据
//
//        Version:  1.0
//        Created:  2013年06月10日 18时37分01秒
//       Revision:  none
//       Compiler:  g++
//
//         Author:  Hurley (LiuHuan), liuhuan1992@gmail.com
//        Company:  Class 1107 of Computer Science and Technology
// 
// =====================================================================================

#include <QtGui>

#include "CamareGet.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QLabel *label = new QLabel();
	CamaraGet *camare = new CamaraGet();

	camare->setLabel(label);
	camare->openCamara();
	
	label->resize(600, 450);
	label->show();

	return app.exec();
}

