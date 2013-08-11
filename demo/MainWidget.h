// =====================================================================================
// 
//       Filename:  MainWidget.h
//
//    Description:  主窗口控件的头文件
//
//        Version:  1.0
//        Created:  2013年06月16日 15时05分40秒
//       Revision:  none
//       Compiler:  g++
//
//         Author:  Hurley (LiuHuan), liuhuan1992@gmail.com
//        Company:  Class 1107 of Computer Science and Technology
// 
// =====================================================================================

#include <QWidget>

class QPushButton;

class MainWidget : public QWidget
{
	Q_OBJECT
public:
	MainWidget();
	~MainWidget();

public slots:
	void on_faceButton_clicked();
	void on_computerButton_clicked();

private:
	QPushButton *faceButton;
	QPushButton *computerButton;
};

