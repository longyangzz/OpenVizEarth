#include "SettingsManager.h"

#include <QApplication>
#include <QMessageBox>
#include <QFile>
#include <QProcess>
#include <QAction>
#include <QMenu>

SettingsManager::SettingsManager(QObject* parent):
	QSettings(parent)
{
}

SettingsManager::~SettingsManager()
{
	sync();
}

void SettingsManager::setOrAddSetting(const QString& key, const QVariant & value)
{
  if (value.isValid())
  {
     setValue(key, value);
    sync();
  }
}

QVariant SettingsManager::getOrAddSetting(const QString& key, const QVariant & defaultValue)
{
  auto  found = value(key);

  if (!found.isValid())
  {
    if (defaultValue.isValid())
    {
		setValue(key, defaultValue);
		sync();
    }

    return defaultValue;
  }

  return found;
}

void  SettingsManager::setupUi(QMenu *menu)
{
  // Reset settings
  QAction *resetAction = new QAction;

  resetAction->setObjectName(QStringLiteral("resetSettingsAction"));
  resetAction->setText(tr("Restore Settings"));
  resetAction->setToolTip("Restore all settings to default");
  resetAction->setStatusTip(resetAction->toolTip());


}

void  SettingsManager::setGlobalSRS(const osgEarth::SpatialReference *globalSRS)
{
	_globalSRS = globalSRS;
}

const osgEarth::SpatialReference * SettingsManager::getGlobalSRS()
{
	return _globalSRS;
}

void  SettingsManager::reset()
{
  int  result = QMessageBox::question(NULL,
                                      tr("Restoring settings"),
                                      tr("About to restore all settings.") + "\n\n" + tr("Requires restarting program, continue?"),
                                      QMessageBox::Yes, QMessageBox::No);

  if (result == QMessageBox::Yes)
  {
		clear();
		qApp->quit();
		QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
  }
}
