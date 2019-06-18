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

//Qt
#include "QApplication"
#include "QFile"
#include "QtSql/QSqlDatabase"
#include "QDebug"
#include "QtSql/QSqlError"
#include "QtSql/QSqlQuery"
#include "QtSql/QSqlRecord"

#include "ConfigParser.h"

bool ConfigParser::ConnectDatabase(QString& databaseName) 
{
	QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE"); 
	

	if (!QFile(databaseName).exists())
	{
		//为了兼容原来的数据库名字，查找是否存在旧版本数据库配置文件
		databaseName = QApplication::applicationDirPath() + "/Config/" + "DCConfig";

		if (!QFile(databaseName).exists())
		{
			return false;
		}
	}

	database.setDatabaseName(databaseName);
	
	//打开数据库
	if(!database.open())
	{  
		qDebug()<<database.lastError();
		//qFatal("failed to connect.") ;
		return false;
	}

	return true;
}

QString ConfigParser::GetGlobalTitle(bool noSpace/* = true*/, bool withVersion/* = false*/)
{
	QSqlQuery query;
	QString title;
	QString version;

	//select Value from Global where Key = "Title"
	QString sql = QString("select Value from Global where Key = '%1' ").arg("Title");

	query.exec(sql);
	//如果存在值则返回该值，不存在，则返回空
	while (query.next())
	{
		title = query.value(0).toString();
	}
	if (noSpace)
	{
		title = title.remove(" ");
	}

	//version
	sql = QString("select Value from Global where Key = '%1' ").arg("Version");

	query.exec(sql);
	//如果存在值则返回该值，不存在，则返回空
	while (query.next())
	{
		version = query.value(0).toString();
	}

	if (!withVersion)
	{
		return title;
	}
	else
	{
		return QString("%1 %2").arg(title).arg(version);
	}

	return "DCLW";
}

QString ConfigParser::GetSoftName(bool withVersion /* = false */)
{
	QSqlQuery query;
	QString title;
	QString version;

	//select Value from Global where Key = "SoftName"
	QString sql = QString("select Value from Global where Key = '%1' ").arg("SoftName");

	query.exec(sql);
	//如果存在值则返回该值，不存在，则返回空
	while (query.next())
	{
		title = query.value(0).toString();
	}


	//version
	sql = QString("select Value from Global where Key = '%1' ").arg("Version");

	query.exec(sql);
	//如果存在值则返回该值，不存在，则返回空
	while (query.next())
	{
		version = query.value(0).toString();
	}

	if (!withVersion)
	{
		return title;
	}
	else
	{
		return QString("%1 %2").arg(title).arg(version);
	}

	return "DC-SoftKit";
}

QString ConfigParser::GetLogoFile()
{
	QSqlQuery query;

	QString sql = QString("select Value from Global where Key = '%1' ").arg("Logo");

	query.exec(sql);
	//如果存在值则返回该值，不存在，则返回空
	while (query.next())
	{
		return query.value(0).toString();
	}

	return "";
}

bool CompareMenu(const Menu &m1, const Menu &m2)
{
	if (m1.classID < m2.classID)
	{
		return true;
	}
	return false;
}

MenuVec ConfigParser::GetMenuVec()
{
	//执行查询语句
	MenuVec mvec;
	Menu currentMenu;
	QSqlQuery query;
	QString sql = "select * from Menu";

	query.exec(sql);
	//如果存在值则返回该值，不存在，则返回空
	while (query.next())
	{
		int recordNum = query.record().count();

		//遍历所有的，根据字段名字给currentAction赋值
		for (int i = 0; i != recordNum; ++i)
		{
			QString fieldName = query.record().fieldName(i);
			int fieldIndex = query.record().indexOf(fieldName);

			if (fieldName.toUpper() == "CLASSID")
			{
				currentMenu.classID = query.value(fieldIndex).toInt();
			}
			else if (fieldName.toUpper() == "OBJECTNAME")
			{
				currentMenu.objectName = query.value(fieldIndex).toString();
			}
			else if (fieldName.toUpper() == "EVENTNAME")
			{
				currentMenu.eventName = query.value(fieldIndex).toString();
			}
			else if (fieldName.toUpper() == "TITLE")
			{
				currentMenu.title = query.value(fieldIndex).toString();
			}
			else if (fieldName.toUpper() == "TOOLBAR")
			{
				currentMenu.toolbar = query.value(fieldIndex).toString();
			}
			else if (fieldName.toUpper() == "TOOLBARTITLE")
			{
				currentMenu.toolBarTitle = query.value(fieldIndex).toString();
			}
			else if (fieldName.toUpper() == "ISSHOW")
			{
				currentMenu.isShow = query.value(fieldIndex).toBool();
			}
			else if (fieldName.toUpper() == "DSC")
			{
				currentMenu.dsc = query.value(fieldIndex).toString();
			}

		}

		mvec.push_back(currentMenu);
	}

	qSort(mvec.begin(), mvec.end(), CompareMenu);
	return mvec;
}

MenuActionVec ConfigParser::GetMenuActionVec(const int& classID)
{
	//执行查询语句
	MenuActionVec actionVec;
	MenuAction currentAction;
	QSqlQuery query;

	//select * from MenuAction where ClassID = 1
	QString sql = QString("select * from MenuAction where ClassID = %1").arg(classID);

	query.exec(sql);
	//如果存在值则返回该值，不存在，则返回空
	while (query.next())
	{
		int recordNum = query.record().count();

		//遍历所有的，根据字段名字给currentAction赋值
		for (int i = 0; i != recordNum; ++i)
		{
			QString fieldName = query.record().fieldName(i);
			int fieldIndex = query.record().indexOf(fieldName);

			if (fieldName.toUpper() == "CLASSID")
			{
				currentAction.classID = query.value(fieldIndex).toInt();
			}
			else if (fieldName.toUpper() == "PARENTMENU")
			{
				currentAction.parentMenu = query.value(fieldIndex).toString();
			}
			else if (fieldName.toUpper() == "ISMENU")
			{
				currentAction.isMenu = query.value(fieldIndex).toBool();
			}
			else if (fieldName.toUpper() == "TEXT")
			{
				currentAction.text = query.value(fieldIndex).toString();
			}
			else if (fieldName.toUpper() == "OBJECTNAME")
			{
				currentAction.objectName = query.value(fieldIndex).toString();
			}
			else if (fieldName.toUpper() == "EVENTNAME")
			{
				currentAction.eventName = query.value(fieldIndex).toString();
			}
			else if (fieldName.toUpper() == "HASTOOLBAR")
			{
				currentAction.hasToolbar = query.value(fieldIndex).toBool();
			}
			else if (fieldName.toUpper() == "TOOLTIP")
			{
				currentAction.tooltip = query.value(fieldIndex).toString();
			}
			else if (fieldName.toUpper() == "TOOLIMAGE")
			{
				currentAction.toolImage = query.value(fieldIndex).toString();
			}
			else if (fieldName.toUpper() == "SEPARATOR")
			{
				currentAction.separator = query.value(fieldIndex).toBool();
			}
			else if (fieldName.toUpper() == "ENABLE")
			{
				currentAction.enable = query.value(fieldIndex).toBool();
			}
			else if (fieldName.toUpper() == "CHECKABLE")
			{
				currentAction.checkable = query.value(fieldIndex).toBool();
			}
			else if (fieldName.toUpper() == "CHECKED")
			{
				currentAction.checked = query.value(fieldIndex).toBool();
			}
			else if (fieldName.toUpper() == "ISSHOW")
			{
				currentAction.isShow = query.value(fieldIndex).toBool();
			}
			else if (fieldName.toUpper() == "DSC")
			{
				currentAction.dsc = query.value(fieldIndex).toString();
			}
		}

		/*int i = 1;
		currentAction.classID = query.value(i++).toInt();
		currentAction.parentMenu = query.value(i++).toString();
		currentAction.isMenu = query.value(i++).toBool();
		currentAction.text = query.value(i++).toString();
		currentAction.objectName = query.value(i++).toString();
		currentAction.eventName = query.value(i++).toString();
		currentAction.hasToolbar = query.value(i++).toBool();
		currentAction.tooltip = query.value(i++).toString();
		currentAction.toolImage = query.value(i++).toString();
		currentAction.separator = query.value(i++).toBool();
		currentAction.enabled = query.value(i++).toBool();
		currentAction.checkable = query.value(i++).toBool();
		currentAction.checked = query.value(i++).toBool();
		currentAction.dsc = query.value(i++).toString();*/

		actionVec.push_back(currentAction);
	}

	return actionVec;
}

int ConfigParser::GetSplashIDStarted()
{
	QSqlQuery query;

	QString sql = QString("select Value from Global where Key = '%1' ").arg("SplashID");

	query.exec(sql);
	//如果存在值则返回该值，不存在，则返回空
	while (query.next())
	{
		return query.value(0).toInt();
	}

	return 1;
};

SplashInfo ConfigParser::GetSplashInfo(int ID)
{
	QSqlQuery query;
	SplashInfo sInfo;
	QString sql = QString("select * from Splash where ID = '%1' ").arg(ID);

	query.exec(sql);
	//如果存在值则返回该值，不存在，则返回空
	while (query.next())
	{
		sInfo.fileName = query.value(1).toString();
		sInfo.time = query.value(2).toInt();
		sInfo.showInfo = query.value(3).toString();
		sInfo.dsc = query.value(4).toString();
	}

	return sInfo;
}