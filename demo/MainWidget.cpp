// =====================================================================================
// 
//       Filename:  MainWidget.cpp
//
//    Description:  主窗口控件
//
//        Version:  1.0
//        Created:  2013年06月16日 15时04分26秒
//       Revision:  none
//       Compiler:  g++
//
//         Author:  Hurley (LiuHuan), liuhuan1992@gmail.com
//        Company:  Class 1107 of Computer Science and Technology
// 
// =====================================================================================

#include <QtGui>
#include <QDebug>

#include "MainWidget.h"

MainWidget::MainWidget()
{
	// 加载背景图片
	QPixmap bgPixmap(":/res/bg.png");
	// 定义画刷	
	QPalette palette;
	palette.setBrush(QPalette::Background, QBrush(bgPixmap));
	
	// 控件安装画刷
	this->setAutoFillBackground(true);
	this->setPalette(palette);

	// 固定设置窗口大小，禁止用户修改
	this->setFixedSize(bgPixmap.size());

	// 脸部的按钮
	faceButton = new QPushButton(this);
	// 设置无焦点（避免按钮周围虚线影响效果）
	faceButton->setFocusPolicy(Qt::NoFocus);
	// 固定按钮位置和大小
	faceButton->setGeometry(QRect(750, 80, 131, 82));
	// 设置按钮的 QSS 属性，前者设置按钮鼠标悬浮图片，后者设置其他状态透明
	faceButton->setStyleSheet("QPushButton:hover{border-image:url(:/res/EyeClose.png);}"
				   "QPushButton{background-color:transparent}");
	connect(faceButton, SIGNAL(clicked()), this, SLOT(on_faceButton_clicked()));
	
	// 我的电脑按钮
	computerButton = new QPushButton(this);
	// 固定按钮位置
	computerButton->setGeometry(QRect(40, 240, 250, 250));
	// 加载按钮图片
    	QPixmap pixmapComputer(":/res/computer.png");
	// 用图片给按钮设置掩码
	computerButton->setMask(pixmapComputer.mask());
	// 设置按钮普通、鼠标悬浮、点击时候的效果
	computerButton->setStyleSheet( "QPushButton{border-image:url(:/res/computer.png);}"
                                   	"QPushButton:hover{border-image:url(:/res/computer_hover.png);}"
                                  	"QPushButton:pressed{border-image:url(:/res/computer_pressed.png);}");
	connect(computerButton, SIGNAL(clicked()), this, SLOT(on_computerButton_clicked()));
}

MainWidget::~MainWidget()
{

}

// 脸部按钮的点击响应槽
void MainWidget::on_faceButton_clicked()
{
	QMessageBox::information(this, tr("嘻嘻～～"), tr("点人家眼睛做什么呀～～"));
}

// 我的电脑按钮的点击响应槽
void MainWidget::on_computerButton_clicked()
{
	QMessageBox::information(this, tr("我的电脑"), tr("我的电脑不对外开放～～"));
}

