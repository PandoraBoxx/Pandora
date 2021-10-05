#include "mainWindow.h"
#include "ui_mainWindow.h"

#include "serialSettings.h"
#include "serialInterface.h"
#include "encryptTool.h"
#include "contactManager.h"
#include "contactKeyboard.h"
#include "messageManager.h"
#include "soundManager.h"
#include "aboutSettings.h"

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QFile>
#include <QDir>

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    m_gui(new Ui::MainWindow)
{
    m_gui->setupUi(this);
    setWindowTitle("Pandora");
    Qt::WindowFlags flags;
    flags |= Qt::FramelessWindowHint;
    flags |= Qt::MSWindowsFixedSizeDialogHint;
    setWindowFlags(flags);

    QDir mediaDir;
    mediaDir.mkdir(QDir::homePath() + "/PandoraContacts");
    m_baseDir = QDir::homePath() + "/PandoraContacts";

    QDir contDir;
    contDir.mkdir(m_baseDir + "/Contacts");
    m_contactDir = m_baseDir + "/Contacts";

    m_ramdiskDir = "/mnt/ramdisk";

    m_aboutSettings = new AboutSettings(this);
    m_encryptTool = new EncryptTool(this);
    m_serialSettings = new SerialSettings(this);
    m_serialInterface = new SerialInterface(this);
    m_contactManager = new ContactManager(this);
    m_contactKeyboard = new ContactKeyboard(this);
    m_messageManager = new MessageManager(this);
    m_soundManager = new SoundManager(this);

    m_gui->selAudioButn->setEnabled(false);
    m_gui->selTextButn->setEnabled(false);
    m_gui->selContactsButn->setEnabled(false);
    m_gui->selMessagesButn->setEnabled(false);
    m_gui->selSettingsButn->setEnabled(false);
    m_gui->setSerialButn->setEnabled(false);
    m_gui->setAboutButn->setEnabled(false);
    m_gui->passwordButn->setHidden(false);
    m_gui->passwordLabel->setHidden(false);
    m_gui->passwordLEdit->setHidden(false);

    connect(m_gui->selAudioButn, SIGNAL(clicked(bool)), this, SLOT(setAudioWidget()));
    connect(m_gui->selContactsButn, SIGNAL(clicked(bool)), this, SLOT(setContactsWidget()));
    connect(m_gui->selMessagesButn, SIGNAL(clicked(bool)), this, SLOT(setMessagesWidget()));
    connect(m_gui->selSettingsButn, SIGNAL(clicked(bool)), this, SLOT(setSettingsWidget()));
    connect(m_gui->cntReceiveButn, SIGNAL(clicked(bool)), this, SLOT(setContactReceiveWidget()));
    connect(m_gui->cntSendButn, SIGNAL(clicked(bool)), this, SLOT(setContactSendWidget()));
    connect(m_gui->msgReceiveButn, SIGNAL(clicked(bool)), this, SLOT(setMessageReceiveWidget()));
    connect(m_gui->msgSendButn, SIGNAL(clicked(bool)), this, SLOT(setMessageSendWidget()));
    connect(m_gui->setSerialButn, SIGNAL(clicked(bool)), this, SLOT(setSettingSerialWidget()));
    connect(m_gui->setOffButn, SIGNAL(clicked(bool)), this, SLOT(setSettingOffFunction()));
    connect(m_gui->setAboutButn, SIGNAL(clicked(bool)), this, SLOT(setSettingAboutWidget()));

    afterInit();
}

MainWindow::~MainWindow()
{
    delete m_gui;
}

void MainWindow::afterInit()
{
    QStringList Tables;
    QSqlError err;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "ContactDatabase");
    db.setDatabaseName(m_baseDir + "/ContactDatabase");
    db.setHostName("");
    db.setPort(-1);
    if (!db.open("", "")) {
        err = db.lastError();
        db = QSqlDatabase();
        QSqlDatabase::removeDatabase("ContactDatabase");
    }

    Tables = db.tables();
    if (!Tables.contains("ContactTable")) {
        QSqlQuery query("", db);
        query.exec("create table ContactTable (ContactName varchar, ContactGroup varchar, ContactDir varchar, ContactDate varchar)");
    }
}

Ui::MainWindow* MainWindow::getGUI()
{
    return m_gui;
}

QString MainWindow::getBaseDir()
{
    return m_baseDir;
}

QString MainWindow::getContactsDir()
{
    return m_contactDir;
}

QString MainWindow::getRamdiskDir()
{
    return m_ramdiskDir;
}

EncryptTool* MainWindow::getEncryptTool()
{
    return m_encryptTool;
}

SerialSettings* MainWindow::getSerialSettings()
{
    return m_serialSettings;
}

SerialInterface* MainWindow::getSerialInterface()
{
    return m_serialInterface;
}

ContactManager* MainWindow::getContactManager()
{
    return m_contactManager;
}

ContactKeyboard* MainWindow::getContactKeyboard()
{
    return m_contactKeyboard;
}

MessageManager* MainWindow::getMessageManager()
{
    return m_messageManager;
}

SoundManager* MainWindow::getSoundManager()
{
    return m_soundManager;
}

AboutSettings* MainWindow::getAboutSettings()
{
    return m_aboutSettings;
}

void MainWindow::setAudioWidget()
{
    m_gui->mainStackedWidget->setCurrentWidget(m_gui->audioPage);
}

void MainWindow::setContactsWidget()
{
    m_gui->mainStackedWidget->setCurrentWidget(m_gui->contactPage);
}

void MainWindow::setMessagesWidget()
{
    m_gui->mainStackedWidget->setCurrentWidget(m_gui->messagePage);
}

void MainWindow::setSettingsWidget()
{
    m_gui->mainStackedWidget->setCurrentWidget(m_gui->settingPage);
}

void MainWindow::setContactReceiveWidget()
{
    m_gui->cntStackedWidget->setCurrentWidget(m_gui->cntReceivePage);
}

void MainWindow::setContactSendWidget()
{
    m_gui->cntStackedWidget->setCurrentWidget(m_gui->cntSendPage);
}

void MainWindow::setMessageReceiveWidget()
{
    m_gui->msgStackedWidget->setCurrentWidget(m_gui->msgReceivePage);
}

void MainWindow::setMessageSendWidget()
{
    m_gui->msgStackedWidget->setCurrentWidget(m_gui->msgSendPage);
}

void MainWindow::setSettingOffFunction()
{
    QFile::remove(m_ramdiskDir + "/localPub.txt");
    QFile::remove(m_ramdiskDir + "/localPriv.txt");
    QFile::remove(m_ramdiskDir + "/localPriv.enc");
    m_contactKeyboard->disconnectQapp();
    qApp->exit();
//    QString file = "sudo poweroff";
//    m_process->start(file);
}

void MainWindow::setSettingSerialWidget()
{
    m_serialInterface->setupSerialPort();
    m_gui->setStackedWidget->setCurrentWidget(m_gui->setSerialPage);
}

void MainWindow::setSettingAboutWidget()
{
    m_gui->setStackedWidget->setCurrentWidget(m_gui->setAboutPage);
}
