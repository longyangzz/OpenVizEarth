//##########################################################################
//#                                                                        #
//#                            DCLW                                        #
//#                                                                        #
//#  This library is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU Lesser General Public License(LGPL) as  #
//#  published by the Free Software Foundation; version 2 of the License.  #
//#                                                                        #
//#  This library is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
//#  GNU Lesser General Public License for more details.                   #
//#                                                                        #
//#          COPYRIGHT: DCLW             								   #
//#                                                                        #
//##########################################################################

/*==========================================================================
*【功能概述】
*
*作者：dclw         时间：$time$
*文件名：$safeitemname$
*版本：V2.1.0
*
*修改者：          时间：
*修改说明：
*===========================================================================
*/

#include "QMainWindow"
#include "QMenuBar"
#include "QMenu"
#include "QAction"
#include "QToolBar"
#include "QMap"

#include "ConfigParser.h"

class UI_IWindow
{
public:
	//变量用来存储创建的所有menu对象
	typedef QMap<QString, QMenu* > MenuMap;
	MenuMap menuMap;

	//Qmainwindow
	QMainWindow* pMainwindow;
	QWidget* centralWidget;
	//菜单栏
	QMenuBar* menuBar;

	//文件菜单
	QMenu* menuFile;
	QAction* actionOpen;

	UI_IWindow(QMainWindow* mainwindow)
	{
		pMainwindow = mainwindow;
		pMainwindow->resize(600, 400);

		centralWidget = new QWidget(mainwindow);
		centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
		mainwindow->setCentralWidget(centralWidget);
	}

	//创建菜单示例代码
	void CreateMenuExample()
	{
		//根菜单
		menuBar = new QMenuBar(pMainwindow);
		menuBar->setObjectName(QString::fromUtf8("menuBar"));
		menuBar->setGeometry(QRect(0, 0, 600, 23));

		//创建文件菜单
		menuFile = new QMenu(menuBar);
		menuFile->setObjectName(QString::fromUtf8("menuFile"));
		menuFile->setTitle("File");

		//为文件菜单添加动作
		actionOpen = new QAction(0);
		actionOpen->setText(QObject::tr("Open"));
		actionOpen->setToolTip(QObject::tr("Open"));
		actionOpen->setObjectName(QString::fromUtf8("actionOpen"));  //要想使用自动信号槽，必须设置该参数
		menuFile->addAction(actionOpen);
		actionOpen->setParent(menuFile);

		//将文件菜单添加到根菜单中
		menuBar->addAction(menuFile->menuAction());

		pMainwindow->setMenuBar(menuBar);

		//创建信号槽的自动连接
		//QMetaObject::connectSlotsByName(pMainwindow);
	}

	void CreateMenuBySqlite()
	{
		//根菜单
		menuBar = new QMenuBar(pMainwindow);
		menuBar->setObjectName(QString::fromUtf8("menuBar"));
		menuBar->setGeometry(QRect(0, 0, 600, 23));

		//从配置文件中获取菜单并创建
		MenuVec mvec = ConfigParser::GetMenuVec();
		int menuNum = mvec.size();

		//i代表一级菜单个数
		for (int i = 0; i != menuNum; ++i)
		{
			//如果设置参数为隐藏，则不显示
			if (!mvec[i].isShow)
			{
				continue;
			}

			QMenu* cm = new QMenu(menuBar);
			menuMap[mvec[i].objectName] = cm;
			cm->setObjectName(mvec[i].objectName);
			cm->setTitle(mvec[i].title);

			//添加信号槽
			if (!mvec[i].eventName.isEmpty())
			{
				QObject::connect(cm, SIGNAL(aboutToShow()), pMainwindow, 
					qFlagLocation( QString("%1%2()").arg(1).arg(mvec[i].eventName).toStdString().c_str() ));
			}

			//是否创建工具栏
			QToolBar* currentToolbar = nullptr;
			if (!mvec[i].toolbar.isEmpty())
			{
				currentToolbar = new QToolBar(pMainwindow);
				currentToolbar->setObjectName(mvec[i].toolbar);
				currentToolbar->setWindowTitle(mvec[i].toolBarTitle);
				pMainwindow->addToolBar(Qt::TopToolBarArea, currentToolbar);
			}

			//为当前菜单添加动作，根据获取的classID到action表中获取所有的动作并添加
			MenuActionVec actionVec = ConfigParser::GetMenuActionVec(mvec[i].classID);
			
			//j代表一级菜单下action个数
			for (int j = 0; j != actionVec.size(); ++j)
			{
				//先判断是否是菜单,是菜单就创建菜单，并记录
				if (actionVec[j].isMenu)
				{
					//创建菜单
					//判断是否创建多级菜单
					if (!actionVec[j].isShow)
					{
						continue;
					}

					//添加到父菜单中,先获取父菜单,父菜单不存在则continue
					if (menuMap.find(actionVec[j].parentMenu) == menuMap.end())
					{
						continue;
					}

					QMenu* sMenu = new QMenu(menuBar);
					menuMap[actionVec[j].objectName] = sMenu;
					sMenu->setObjectName(actionVec[j].objectName);
					sMenu->setTitle(actionVec[j].text);

					//添加信号槽
					if (!actionVec[j].eventName.isEmpty())
					{
						QObject::connect(sMenu, SIGNAL(aboutToShow()), pMainwindow, 
							qFlagLocation( QString("%1%2()").arg(1).arg(actionVec[j].eventName).toStdString().c_str() ));
					}

					
					QMenu* pMenu = menuMap[actionVec[j].parentMenu];
					//判断caction前边是否添加分隔符
					if (actionVec[j].separator)
					{
						pMenu->addSeparator();
					}
					pMenu->addAction(sMenu->menuAction());
				}
				else //不是菜单肯定是action
				{
					//如果设置参数为隐藏，则不显示
					if (!actionVec[j].isShow)
					{
						continue;
					}

					//添加到父菜单中,先获取父菜单,父菜单不存在则continue
					if (menuMap.find(actionVec[j].parentMenu) == menuMap.end())
					{
						continue;
					}

					QAction* caction = new QAction(0);
					caction->setText(actionVec[j].text);
					caction->setObjectName(actionVec[j].objectName);

					//添加信号槽
					if (!actionVec[j].eventName.isEmpty())
					{
						QObject::connect(caction, SIGNAL(triggered()), pMainwindow, 
							qFlagLocation( QString("%1%2()").arg(1).arg(actionVec[j].eventName).toStdString().c_str() ));
					}

					//工具栏处理
					if (actionVec[j].hasToolbar && currentToolbar)
					{
						caction->setToolTip(actionVec[j].tooltip);

						QString img = actionVec[j].toolImage;
						QString imgFile;
						if (img.at(0) != QLatin1Char(':'))
						{
							imgFile = QString("%1/%2").arg(QApplication::applicationDirPath()).arg(img);
						}
						else
						{
							imgFile = img;
						}
						QIcon icon1;
						icon1.addFile(imgFile, QSize(), QIcon::Normal, QIcon::Off);
						caction->setIcon(icon1);

						if (actionVec[j].separator)
						{
							currentToolbar->addSeparator();
						}

						currentToolbar->addAction(caction);
					}

					//属性设置
					caction->setEnabled(actionVec[j].enable);
					caction->setCheckable(actionVec[j].checkable);
					caction->setChecked(actionVec[j].checked);

					QMenu* pMenu = menuMap[actionVec[j].parentMenu];
					//判断caction前边是否添加分隔符
					if (actionVec[j].separator)
					{
						pMenu->addSeparator();
					}
					
					pMenu->addAction(caction);
					caction->setParent(pMenu);
				}
				
			}

			//将单前菜单添加到菜单栏中
			menuBar->addAction(cm->menuAction());
		}

		pMainwindow->setMenuBar(menuBar);

		//创建信号槽的自动连接,不在这里链接，在子类中调用
		//QMetaObject::connectSlotsByName(pMainwindow);
	}
};