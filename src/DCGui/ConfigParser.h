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
//#          COPYRIGHT: DCLW             																	 #
//#                                                                        #
//##########################################################################

/*==========================================================================
*【功能概述】解析sqlite配置文件，创建GUI，暂定为本库内部使用
*
*作者：dclw         时间：%time%
*文件名：%safeitemname%
*版本：V2.1.0
*$safeitemrootname$
*修改者：          时间：
*修改说明：
*===========================================================================
*/

#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H

//Qt
#include <QObject>
#include "QVector"

/********************splash信息***************************************************/
class SplashInfo
{
public:
	SplashInfo()
	{

	}

	QString fileName;
	int time;
	QString showInfo;
	QString dsc;
};

/********************根菜单数据结构***************************************************/
class Menu
{
public:
	Menu()
		: isShow(true)
	{

	}

public:
	int classID;
	QString objectName;
	QString eventName;
	QString title;
	QString toolbar;
	QString toolBarTitle;
	bool isShow;
	QString dsc;
};

/********************action数据结构***************************************************/
class MenuAction
{
public:
	MenuAction()
		: isMenu(false)
		, hasToolbar(false)
		, separator(false)
		, enable(true)
		, checkable(false)
		, checked(false)
		, isShow(true)
	{

	}

public:
	int classID;
	QString parentMenu;
	bool isMenu;
	QString text;
	QString objectName;
	QString eventName;
	bool hasToolbar;
	QString tooltip;
	QString toolImage;
	bool separator;
	bool enable;
	bool checkable;
	bool checked;
	bool isShow;
	QString dsc;
};

typedef QVector<Menu > MenuVec;
typedef QVector<MenuAction > MenuActionVec;

namespace ConfigParser
{
	//数据库操作
	bool ConnectDatabase(QString& databaseName);
	
	/*****************数据库查询结果*********************************************/
	//UI标题
	QString GetGlobalTitle(bool noSpace = true, bool withVersion = false);

	QString GetSoftName(bool withVersion = false);

	//UI logofile
	QString GetLogoFile();
	
	MenuVec GetMenuVec();
	MenuActionVec GetMenuActionVec(const int& classID);

	//splash 信息
	int GetSplashIDStarted();
	SplashInfo GetSplashInfo(int ID);
}

#endif // CONFIGPARSER_H
