#include "mainWindow.h"
#include "ui_mainWindow.h"

#include "serialSettings.h"
#include "serialInterface.h"
#include "encryptTool.h"
#include "contactManager.h"
#include "messageManager.h"
#include "soundManager.h"

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDir>

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    m_gui(new Ui::MainWindow)
{
    m_gui->setupUi(this);
    setWindowTitle("Pandora");

    m_encryptTool = new EncryptTool(this);
    m_serialSettings = new SerialSettings(this);
    m_serialInterface = new SerialInterface(this);
    m_contactManager = new ContactManager(this);
    m_messageManager = new MessageManager(this);
    m_soundManager = new SoundManager(this);

    m_baseDir = QDir::currentPath();

    QPixmap aPixmap(m_baseDir + "/../Src/pandora.png");
    m_gui->pictureLabel->setAttribute(Qt::WA_TranslucentBackground);
    m_gui->pictureLabel->setPixmap(aPixmap);

    connect(m_gui->selAudioButn, SIGNAL(clicked(bool)), this, SLOT(setAudioWidget()));
    connect(m_gui->selContactsButn, SIGNAL(clicked(bool)), this, SLOT(setContactsWidget()));
    connect(m_gui->selMessagesButn, SIGNAL(clicked(bool)), this, SLOT(setMessagesWidget()));
    connect(m_gui->selSettingsButn, SIGNAL(clicked(bool)), this, SLOT(setSettingsWidget()));
    connect(m_gui->cntReceiveButn, SIGNAL(clicked(bool)), this, SLOT(setContactReceiveWidget()));
    connect(m_gui->cntSendButn, SIGNAL(clicked(bool)), this, SLOT(setContactSendWidget()));
    connect(m_gui->msgReceiveButn, SIGNAL(clicked(bool)), this, SLOT(setMessageReceiveWidget()));
    connect(m_gui->msgSendButn, SIGNAL(clicked(bool)), this, SLOT(setMessageSendWidget()));
    connect(m_gui->setSerialButn, SIGNAL(clicked(bool)), this, SLOT(setSettingSerialWidget()));
    connect(m_gui->setAboutButn, SIGNAL(clicked(bool)), this, SLOT(setSettingAboutWidget()));

    afterInit();
}

MainWindow::~MainWindow()
{
    delete m_gui;
}

void MainWindow::afterInit()
{
    QByteArray path1, path2;
    bool exPath1, exPath2;
    QStringList Tables;
    QSqlError err;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "ContactDatabase");
    db.setDatabaseName("ContactDatabase");
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

    path1 = m_baseDir.toUtf8() + "/localPub.txt";
    path2 = m_baseDir.toUtf8() + "/localPriv.txt";;
    exPath1 = QFileInfo::exists(path1);
    exPath2 = QFileInfo::exists(path2);
    if (exPath1 && exPath2) {
        loadLocalKeys();
    } else {
        generateLocalKeys();
    }

    int result;
    unsigned char* message = nullptr;

    result = m_encryptTool->readFile(path1.data(), &message);
    if (result == -1) {
        qDebug() << "Failed to load localPub data";
        if (message != nullptr) free(message);
        return;
    }
    size_t messageLength = static_cast<unsigned int>(result);
    unsigned char* hashed = nullptr;
    size_t hashedSiz = 0;

    if (!m_encryptTool->computeHash(reinterpret_cast<char*>(message), messageLength, &hashed, &hashedSiz, 1)) {
        qDebug() << "Signature failed";
        if (message != nullptr)          free(message);
        if (hashed != nullptr)           OPENSSL_free(hashed);
        return;
    }

    QByteArray rfname = QByteArray::fromRawData(reinterpret_cast<const char*>(hashed), hashedSiz);
    m_pKeyHash.clear();
    for (int idx=0; idx<rfname.size(); idx++) {
        int numb = rfname.at(idx) & 0xff;
        m_pKeyHash.append(QString::number(numb, 16).toUpper());
    }

    /* Clean up */
    if (message != nullptr)          free(message);
    if (hashed != nullptr)           OPENSSL_free(hashed);
}

void MainWindow::loadLocalKeys()
{
    int result;
    QByteArray path;

    /* Read the local public key */
    unsigned char* file1 = nullptr;
    path = m_baseDir.toUtf8() + "/localPub.txt";
    result = m_encryptTool->readFile(path.data(), &file1);
    if (result == -1) {
        qDebug() << "Failed to load localPub.txt keys";
        if (file1 != nullptr) free(file1);
        return;
    }
    size_t fileLength1 = static_cast<unsigned int>(result);
    m_encryptTool->setLocalPublicKey(file1, fileLength1);

    /* Read the local private key */
    unsigned char* file0 = nullptr;
    path = m_baseDir.toUtf8() + "/localPriv.txt";
    result = m_encryptTool->readFile(path.data(), &file0);
    if (result == -1) {
        qDebug() << "Failed to load localPriv.txt keys";
        if (file0 != nullptr) free(file0);
        if (file1 != nullptr) free(file1);
        return;
    }
    size_t fileLength0 = static_cast<unsigned int>(result);
    m_encryptTool->setLocalPrivateKey(file0, fileLength0);

    if (file0 != nullptr) free(file0);
    if (file1 != nullptr) free(file1);
}

void MainWindow::generateLocalKeys()
{
    QString path;

    path = m_baseDir + "/localPriv.txt";
    FILE* fd0 = fopen(path.toUtf8(), "wb");
    if(fd0 == nullptr) {
        qDebug() << "Failed to open localPriv.txt file: " << strerror(errno);
        return;
    }
    path = m_baseDir + "/localPub.txt";
    FILE* fd1 = fopen(path.toUtf8(), "wb");
    if(fd1 == nullptr) {
        qDebug() << "Failed to open localPub.txt file: " << strerror(errno);
        fclose(fd0);
        return;
    }

    m_encryptTool->generateRsa();
    m_encryptTool->writeKeyToFile(fd0, KEY_SERVER_PRI);
    m_encryptTool->writeKeyToFile(fd1, KEY_SERVER_PUB);

    fclose(fd0);
    fclose(fd1);
}

QString MainWindow::getOwnPKeyHash()
{
    return m_pKeyHash;
}

Ui::MainWindow* MainWindow::getGUI()
{
    return m_gui;
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

MessageManager* MainWindow::getMessageManager()
{
    return m_messageManager;
}

SoundManager* MainWindow::getSoundManager()
{
    return m_soundManager;
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

void MainWindow::setSettingSerialWidget()
{
    m_serialInterface->setupSerialPort();
    m_gui->setStackedWidget->setCurrentWidget(m_gui->setSerialPage);
}

void MainWindow::setSettingAboutWidget()
{
    m_gui->setStackedWidget->setCurrentWidget(m_gui->setAboutPage);
}
