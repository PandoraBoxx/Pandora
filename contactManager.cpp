#include "contactManager.h"
#include "mainWindow.h"
#include "ui_mainWindow.h"
#include "encryptTool.h"
#include "progressBar.h"
#include "serialInterface.h"

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDir>
#include <QDateTime>
#include <QDebug>

ContactManager::ContactManager(QObject* parent) : QObject(parent)
{
    m_mainWindow = reinterpret_cast<MainWindow*>(parent);
    m_gui = m_mainWindow->getGUI();
    m_encryptTool = m_mainWindow->getEncryptTool();
    m_serialInterface = m_mainWindow->getSerialInterface();
    m_receivingStatus = false;
    m_sendingStatus = false;
    m_timer = new QTimer(this);

    m_gui->cntRecResultLabel->setWordWrap(true);
    m_gui->cntSendResultLabel->setWordWrap(true);
    m_gui->cntNameLEdit->setDisabled(true);
    m_gui->cntGroupLEdit->setDisabled(true);
    m_gui->cntAcceptButn->setDisabled(true);
    QRegularExpression rxv1("[A-Za-z0-9][A-Za-z0-9 ]{0,15}");
    QRegularExpression rxv2("[A-Za-z0-9]{0,16}");
    QValidator* validator1 = new QRegularExpressionValidator(rxv1, this);
    QValidator* validator2 = new QRegularExpressionValidator(rxv2, this);
    m_gui->cntNameLEdit->setValidator(validator1);
    m_gui->cntGroupLEdit->setValidator(validator2);

    ProgressBar* cntRemoteSize = new ProgressBar(m_gui->cntReceivePage);
    m_cntRemoteSizeList.append(cntRemoteSize);
    m_gui->cntRecProgressHLayout->addWidget(cntRemoteSize);

    ProgressBar* cntLocalSize = new ProgressBar(m_gui->cntSendPage);
    m_cntLocalSizeList.append(cntLocalSize);
    m_gui->cntSendProgressHLayout->addWidget(cntLocalSize);

    connect(m_gui->cntDeleteButn, &QPushButton::clicked, this, &ContactManager::deleteContact);
    connect(m_gui->cntAcceptButn, &QPushButton::clicked, this, &ContactManager::acceptContact);
    connect(m_gui->cntCancelButn, &QPushButton::clicked, this, &ContactManager::cancelContact);
    connect(m_gui->cntGroupCbBox,  QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ContactManager::groupSelected);
    connect(m_gui->cntNameCbBox,  QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ContactManager::nameSelected);
    connect(m_serialInterface, &SerialInterface::cntRemotLengthSignal, this, &ContactManager::remoteContactProgress);
    connect(m_serialInterface, &SerialInterface::cntLocalLengthSignal, this, &ContactManager::localContactProgress);
    connect(m_serialInterface, &SerialInterface::cntReceivedSignal, this, &ContactManager::contactKeyReceived);
    connect(m_serialInterface, &SerialInterface::cntSentSignal, this, &ContactManager::contactKeySent);
    connect(m_gui->mainStackedWidget, &QStackedWidget::currentChanged, this, &ContactManager::pageChanged);
    connect(m_gui->cntStackedWidget, &QStackedWidget::currentChanged, this, &ContactManager::pageChanged);
    connect(m_gui->cntSendButn, &QPushButton::clicked, this, &ContactManager::sendPublicKey);
    connect(m_timer, &QTimer::timeout, this, &ContactManager::checkKeySent);
}

ContactManager::~ContactManager()
{
}

void ContactManager::pageChanged()
{
    if (m_gui->cntReceivePage->isVisible()) {
        m_serialInterface->recvKey();
        m_receivingStatus = true;
        updateContactList();
        m_gui->cntRecResultLabel->setText("Ready to receive new contacts.");
    } else {
        if (m_receivingStatus) {
            if (!m_gui->msgReceivePage->isVisible()) {
                m_serialInterface->stopReceiving();
            }
            m_receivingStatus = false;
        }
    }
    if (m_gui->cntSendPage->isVisible()) {
        if (!m_sendingStatus) {
            m_gui->cntSendResultLabel->setText("Ready to send the key.");
        }
    }
}

void ContactManager::acceptContact()
{
    QString nBox = m_gui->cntNameCbBox->currentText();
    QString gBox = m_gui->cntGroupCbBox->currentText();
    QString group = m_gui->cntGroupLEdit->text();
    QString name = m_gui->cntNameLEdit->text();
    QString folder;
    if (nBox == "New Name") {
        if (name == "") {
            m_gui->cntRecResultLabel->setText("Please enter a new name or change the box selection.");
            return;
        } else {
            nBox = name;
        }
    }
    if (gBox == "New Group") {
        if (group == "") {
            m_gui->cntRecResultLabel->setText("Please enter a new group or change the box selection.");
            return;
        } else {
            gBox = group;
        }
    }

    int result;
    unsigned char* message = nullptr;
    QByteArray path;

    QString baseDir = QDir::currentPath();
    path = baseDir.toUtf8() + "/remotePubb.txt";
    result = m_encryptTool->readFile(path.data(), &message);
    if (result == -1) {
        m_gui->cntRecResultLabel->setText("Failed to load remotePubb data.");
        if (message != nullptr) free(message);

        /************************************/
        m_cntRemoteSizeList.at(0)->setLevel(0);
        m_gui->cntAcceptButn->setDisabled(true);
        m_serialInterface->recvKey();
        updateContactList();
        /************************************/

        return;
    }
    size_t messageLength = static_cast<unsigned int>(result);
    unsigned char* hashed = nullptr;
    size_t hashedSiz = 0;

    if (!m_encryptTool->computeHash(reinterpret_cast<char*>(message), messageLength, &hashed, &hashedSiz, 1)) {
        m_gui->cntRecResultLabel->setText("Failed to calculate the signature.");
        if (message != nullptr)          free(message);
        if (hashed != nullptr)           OPENSSL_free(hashed);

        /************************************/
        m_cntRemoteSizeList.at(0)->setLevel(0);
        m_gui->cntAcceptButn->setDisabled(true);
        m_serialInterface->recvKey();
        updateContactList();
        /************************************/

        return;
    }
    QByteArray rfname = QByteArray::fromRawData(reinterpret_cast<const char*>(hashed), hashedSiz);
    QString fname;
    for (int idx=0; idx<rfname.size(); idx++) {
        int numb = rfname.at(idx) & 0xff;
        fname.append(QString::number(numb, 16).toUpper());
    }

    QDir contDir;
    QString srcfname = QDir::currentPath() + "/remotePubb.txt";
    contDir.mkdir(QDir::currentPath() + "/Contacts");
    QString dstfname = QDir::currentPath() + "/Contacts/" + fname;

    QSqlDatabase db = QSqlDatabase::database("ContactDatabase");
    QSqlQuery query(db);
    QString ftext = "select ContactName, ContactGroup, ContactDir from ContactTable where ContactDir='" + dstfname + "'";
    query.exec(ftext);
    while (query.next()) {
        name = query.value(0).toString();
        group = query.value(1).toString();
        folder = query.value(2).toString();
        if (dstfname == folder) {
            if (name != nBox && group != gBox) {
                m_gui->cntRecResultLabel->setText("Warning there is already a contact named '" + name + "' in the group '" + group + "' with the same key.");
                return;
            } else if (name != nBox && group == gBox) {
                m_gui->cntRecResultLabel->setText("Warning there is already a contact named '" + name + "' in the group '" + group + "' with the same key.");
                return;
            } else if (name == nBox && group != gBox) {
                m_gui->cntRecResultLabel->setText("Warning there is already a contact named '" + name + "' in the group '" + group + "' with the same key.");
                return;
            } else {
                m_gui->cntRecResultLabel->setText("Warning the contact already exists.");
                return;
            }
        }
    }

    QFile::copy(srcfname, dstfname);
    if (QFile::exists(dstfname)) QFile::remove(srcfname);
    QFileInfo info(dstfname);
    QString fdate = info.lastModified().toString();

    ftext = "select ContactDir from ContactTable where ContactName='" + nBox + "' and ContactGroup='" + gBox + "'";
    query.exec(ftext);
    while (query.next()) {
        folder = query.value(0).toString();
        if (!QFile::remove(folder)) {
            qDebug() << "the key is not on the media";
        }
    }

    ftext = "delete from ContactTable where ContactName='" + nBox + "' and ContactGroup='" + gBox + "'";
    query.exec(ftext);
    ftext = "insert into ContactTable values ('" + nBox + "', '" + gBox + "', '" + dstfname + "', '" + fdate + "')";
    query.exec(ftext);

    if (message != nullptr)          free(message);
    if (hashed != nullptr)           OPENSSL_free(hashed);

    m_gui->cntRecResultLabel->setText("Contact accepted!.");

    /************************************/
    m_cntRemoteSizeList.at(0)->setLevel(0);
    m_gui->cntAcceptButn->setDisabled(true);
    m_serialInterface->recvKey();
    updateContactList();
    /************************************/
}

void ContactManager::deleteContact()
{
    QString nBox = m_gui->cntNameCbBox->currentText();
    QString gBox = m_gui->cntGroupCbBox->currentText();
    QString group = m_gui->cntGroupLEdit->text();
    QString name = m_gui->cntNameLEdit->text();
    QString folder;
    if (nBox == "New Name") {
        if (name == "") {
            m_gui->cntRecResultLabel->setText("Please enter a name or change the box selection.");
            return;
        } else {
            nBox = name;
        }
    }
    if (gBox == "New Group") {
        if (group == "") {
            m_gui->cntRecResultLabel->setText("Please enter a group or change the box selection.");
            return;
        } else {
            gBox = group;
        }
    }

    QSqlDatabase db = QSqlDatabase::database("ContactDatabase");
    QSqlQuery query(db);
    QString ftext = "select ContactDir from ContactTable where ContactName='" + nBox + "' and ContactGroup='" + gBox + "'";
    query.exec(ftext);
    int qsize = 0;
    while (query.next()) {
        qsize++;
        folder = query.value(0).toString();
        if (!QFile::remove(folder)) {
            qDebug() << "the key is not on the media";
        }
    }

    ftext = "delete from ContactTable where ContactName='" + nBox + "' and ContactGroup='" + gBox + "'";
    query.exec(ftext);
    if (qsize) {
        m_gui->cntRecResultLabel->setText("The contact by the name: '" + nBox + "' in group: '" + gBox + "' was deleted.");
    } else {
        m_gui->cntRecResultLabel->setText("There is no contact by the name: '" + nBox + "' in group: '" + gBox + "'.");
    }
    updateContactList();
}

void ContactManager::cancelContact()
{
    if (m_gui->cntAcceptButn->isEnabled()) m_gui->cntRecResultLabel->setText("Contact cancelled!.");
    else m_gui->cntRecResultLabel->clear();

    /************************************/
    m_cntRemoteSizeList.at(0)->setLevel(0);
    m_gui->cntAcceptButn->setDisabled(true);
    m_serialInterface->recvKey();
    updateContactList();
    /************************************/

}

void ContactManager::nameSelected(int idx)
{
    QString text;

    if (idx == -1)
        return;

    m_gui->cntRecResultLabel->clear();
    text = m_gui->cntNameCbBox->itemText(idx);
    if (text.contains("New Name")) {
        m_gui->cntNameLEdit->clear();
        m_gui->cntNameLEdit->setDisabled(false);
    } else {
        m_gui->cntNameLEdit->setDisabled(true);
    }
}

void ContactManager::groupSelected(int idx)
{
    QString text, name, ftext;

    if (idx == -1)
        return;

    m_gui->cntNameCbBox->clear();
    m_gui->cntRecResultLabel->clear();

    text = m_gui->cntGroupCbBox->itemText(idx);
    QSqlDatabase db = QSqlDatabase::database("ContactDatabase");
    QSqlQuery query(db);
    if (!text.contains("New Group")) {
        m_gui->cntGroupLEdit->setDisabled(true);
        ftext = "select ContactName from ContactTable where ContactGroup='" + text + "'";
    } else {
        m_gui->cntGroupLEdit->clear();
        m_gui->cntGroupLEdit->setDisabled(false);
        ftext = "select ContactName from ContactTable";
    }
    name = "New Name";
    m_gui->cntNameCbBox->addItem(QIcon(), name);
    query.exec(ftext);
    while (query.next()) {
        name = query.value(0).toString();
        m_gui->cntNameCbBox->addItem(QIcon(), name);
    }
}

void ContactManager::updateContactList()
{
    QString name;

    disconnect(m_gui->cntGroupCbBox,  QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ContactManager::groupSelected);
    disconnect(m_gui->cntNameCbBox,  QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ContactManager::nameSelected);

    m_gui->cntNameCbBox->clear();
    name = "New Name";
    m_gui->cntNameCbBox->addItem(QIcon(), name);
    m_gui->cntNameLEdit->clear();
    m_gui->cntNameLEdit->setDisabled(false);

    m_gui->cntGroupCbBox->clear();
    name = "New Group";
    m_gui->cntGroupCbBox->addItem(QIcon(), name);
    m_gui->cntGroupLEdit->clear();
    m_gui->cntGroupLEdit->setDisabled(false);

    QSqlDatabase db = QSqlDatabase::database("ContactDatabase");
    QSqlQuery query(db);
    query.exec("select DISTINCT ContactGroup from ContactTable");
    while (query.next()) {
        name = query.value(0).toString();
        m_gui->cntGroupCbBox->addItem(QIcon(), name);
    }

    connect(m_gui->cntGroupCbBox,  QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ContactManager::groupSelected);
    connect(m_gui->cntNameCbBox,  QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ContactManager::nameSelected);
}

void ContactManager::remoteContactProgress(qreal length, QString type)
{
    m_cntRemoteSizeList.at(0)->setLevel(length);
    if (length < 1.0F) {
        m_gui->messageLabel->setText("Receiving " + type + "...");
    } else {
        m_gui->messageLabel->setText(type + " Received");
    }
}

void ContactManager::localContactProgress(qreal length, QString type)
{
    m_cntLocalSizeList.at(0)->setLevel(length);
    if (length < 1.0F) {
        m_gui->messageLabel->setText("Sending " + type + "...");
    } else {
        m_gui->messageLabel->setText(type + " Sent");
    }
}

void ContactManager::contactKeyReceived()
{
    m_gui->cntRecResultLabel->setText("A key was received, please enter contact details.");
    m_gui->cntAcceptButn->setDisabled(false);
}

void ContactManager::contactKeySent()
{
    m_gui->cntSendResultLabel->setText("The local key was sent.");
    m_timer->start(2000);
}

void ContactManager::checkKeySent()
{
    m_gui->cntSendResultLabel->setText("Ready to send the key.");

    m_cntLocalSizeList.at(0)->setLevel(0);
    m_gui->cntSendButn->setEnabled(true);
    m_gui->msgTransmitButn->setEnabled(true);
    m_gui->messageLabel->clear();
    m_sendingStatus = false;
    m_timer->stop();
}

void ContactManager::sendPublicKey()
{
    m_sendingStatus = true;
    m_gui->cntSendButn->setEnabled(false);
    m_gui->msgTransmitButn->setEnabled(false);
    m_gui->cntSendResultLabel->clear();
    m_serialInterface->sendKey();
}
