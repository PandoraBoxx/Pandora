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
    m_capsLock = true;

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

    connect(m_gui->num0Butn, &QPushButton::clicked, this, &ContactManager::num0Char);
    connect(m_gui->num1Butn, &QPushButton::clicked, this, &ContactManager::num1Char);
    connect(m_gui->num2Butn, &QPushButton::clicked, this, &ContactManager::num2Char);
    connect(m_gui->num3Butn, &QPushButton::clicked, this, &ContactManager::num3Char);
    connect(m_gui->num4Butn, &QPushButton::clicked, this, &ContactManager::num4Char);
    connect(m_gui->num5Butn, &QPushButton::clicked, this, &ContactManager::num5Char);
    connect(m_gui->num6Butn, &QPushButton::clicked, this, &ContactManager::num6Char);
    connect(m_gui->num7Butn, &QPushButton::clicked, this, &ContactManager::num7Char);
    connect(m_gui->num8Butn, &QPushButton::clicked, this, &ContactManager::num8Char);
    connect(m_gui->num9Butn, &QPushButton::clicked, this, &ContactManager::num9Char);
    connect(m_gui->letterAButn, &QPushButton::clicked, this, &ContactManager::letterAChar);
    connect(m_gui->letterBButn, &QPushButton::clicked, this, &ContactManager::letterBChar);
    connect(m_gui->letterCButn, &QPushButton::clicked, this, &ContactManager::letterCChar);
    connect(m_gui->letterDButn, &QPushButton::clicked, this, &ContactManager::letterDChar);
    connect(m_gui->letterEButn, &QPushButton::clicked, this, &ContactManager::letterEChar);
    connect(m_gui->letterFButn, &QPushButton::clicked, this, &ContactManager::letterFChar);
    connect(m_gui->letterGButn, &QPushButton::clicked, this, &ContactManager::letterGChar);
    connect(m_gui->letterHButn, &QPushButton::clicked, this, &ContactManager::letterHChar);
    connect(m_gui->letterIButn, &QPushButton::clicked, this, &ContactManager::letterIChar);
    connect(m_gui->letterJButn, &QPushButton::clicked, this, &ContactManager::letterJChar);
    connect(m_gui->letterKButn, &QPushButton::clicked, this, &ContactManager::letterKChar);
    connect(m_gui->letterLButn, &QPushButton::clicked, this, &ContactManager::letterLChar);
    connect(m_gui->letterMButn, &QPushButton::clicked, this, &ContactManager::letterMChar);
    connect(m_gui->letterNButn, &QPushButton::clicked, this, &ContactManager::letterNChar);
    connect(m_gui->letterOButn, &QPushButton::clicked, this, &ContactManager::letterOChar);
    connect(m_gui->letterPButn, &QPushButton::clicked, this, &ContactManager::letterPChar);
    connect(m_gui->letterQButn, &QPushButton::clicked, this, &ContactManager::letterQChar);
    connect(m_gui->letterRButn, &QPushButton::clicked, this, &ContactManager::letterRChar);
    connect(m_gui->letterSButn, &QPushButton::clicked, this, &ContactManager::letterSChar);
    connect(m_gui->letterTButn, &QPushButton::clicked, this, &ContactManager::letterTChar);
    connect(m_gui->letterUButn, &QPushButton::clicked, this, &ContactManager::letterUChar);
    connect(m_gui->letterVButn, &QPushButton::clicked, this, &ContactManager::letterVChar);
    connect(m_gui->letterWButn, &QPushButton::clicked, this, &ContactManager::letterWChar);
    connect(m_gui->letterXButn, &QPushButton::clicked, this, &ContactManager::letterXChar);
    connect(m_gui->letterYButn, &QPushButton::clicked, this, &ContactManager::letterYChar);
    connect(m_gui->letterZButn, &QPushButton::clicked, this, &ContactManager::letterZChar);
    connect(m_gui->spaceButn, &QPushButton::clicked, this, &ContactManager::letterSpace);
    connect(qApp, &QApplication::focusChanged, this, &ContactManager::focusFunction);
    connect(m_gui->capsButn, &QPushButton::clicked, this, &ContactManager::capsFunction);
    connect(m_gui->hideButn, &QPushButton::clicked, this, &ContactManager::hideFunction);
    connect(m_gui->clearButn, &QPushButton::clicked, this, &ContactManager::clearFunction);
}

ContactManager::~ContactManager()
{
}

void ContactManager::num0Char()
{
    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("0");
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("0");
}

void ContactManager::num1Char()
{
    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("1");
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("1");
}

void ContactManager::num2Char()
{
    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("2");
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("2");
}

void ContactManager::num3Char()
{
    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("3");
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("3");
}

void ContactManager::num4Char()
{
    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("4");
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("4");
}

void ContactManager::num5Char()
{
    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("5");
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("5");
}

void ContactManager::num6Char()
{
    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("6");
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("6");
}

void ContactManager::num7Char()
{
    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("7");
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("7");
}

void ContactManager::num8Char()
{
    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("8");
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("8");
}

void ContactManager::num9Char()
{
    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("9");
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("9");
}

void ContactManager::letterAChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("A");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("A");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("a");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("a");
    }
}

void ContactManager::letterBChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("B");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("B");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("b");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("b");
    }
}

void ContactManager::letterCChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("C");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("C");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("c");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("c");
    }
}

void ContactManager::letterDChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("D");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("D");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("d");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("d");
    }
}

void ContactManager::letterEChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("E");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("E");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("e");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("e");
    }
}

void ContactManager::letterFChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("F");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("F");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("f");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("f");
    }
}

void ContactManager::letterGChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("G");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("G");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("g");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("g");
    }
}

void ContactManager::letterHChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("H");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("H");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("h");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("h");
    }
}

void ContactManager::letterIChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("I");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("I");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("i");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("i");
    }
}

void ContactManager::letterJChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("J");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("J");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("j");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("j");
    }
}

void ContactManager::letterKChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("K");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("K");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("k");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("k");
    }
}

void ContactManager::letterLChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("L");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("L");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("l");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("l");
    }
}

void ContactManager::letterMChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("M");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("M");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("m");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("m");
    }
}

void ContactManager::letterNChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("N");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("N");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("n");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("n");
    }
}

void ContactManager::letterOChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("O");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("O");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("o");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("o");
    }
}

void ContactManager::letterPChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("P");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("P");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("p");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("p");
    }
}

void ContactManager::letterQChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("Q");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("Q");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("q");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("q");
    }
}

void ContactManager::letterRChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("R");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("R");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("r");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("r");
    }
}

void ContactManager::letterSChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("S");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("S");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("s");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("s");
    }
}

void ContactManager::letterTChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("T");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("T");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("t");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("t");
    }
}

void ContactManager::letterUChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("U");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("U");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("u");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("u");
    }
}

void ContactManager::letterVChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("V");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("V");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("v");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("v");
    }
}

void ContactManager::letterWChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("W");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("W");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("w");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("w");
    }
}

void ContactManager::letterXChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("X");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("X");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("x");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("x");
    }
}

void ContactManager::letterYChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("Y");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("Y");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("y");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("y");
    }
}

void ContactManager::letterZChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("Z");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("Z");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("z");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("z");
    }
}

void ContactManager::letterSpace()
{
    if (m_gui->cntNameLEdit->hasFocus())
        m_gui->cntNameLEdit->insert(" ");
    else if (m_gui->cntGroupLEdit->hasFocus())
        m_gui->cntGroupLEdit->insert(" ");
}

void ContactManager::focusFunction(QWidget* old, QWidget* now)
{
    if (now == m_gui->cntNameLEdit || now == m_gui->cntGroupLEdit) m_gui->keyboardFrame->show();
}

void ContactManager::capsFunction()
{
    if (m_capsLock) {
        m_gui->letterAButn->setText("a"); m_gui->letterBButn->setText("b"); m_gui->letterCButn->setText("c"); m_gui->letterDButn->setText("d");
        m_gui->letterEButn->setText("e"); m_gui->letterFButn->setText("f"); m_gui->letterGButn->setText("g"); m_gui->letterHButn->setText("h");
        m_gui->letterIButn->setText("i"); m_gui->letterJButn->setText("j"); m_gui->letterKButn->setText("k"); m_gui->letterLButn->setText("l");
        m_gui->letterMButn->setText("m"); m_gui->letterNButn->setText("n"); m_gui->letterOButn->setText("o"); m_gui->letterPButn->setText("p");
        m_gui->letterQButn->setText("q"); m_gui->letterRButn->setText("r"); m_gui->letterSButn->setText("s"); m_gui->letterTButn->setText("t");
        m_gui->letterUButn->setText("u"); m_gui->letterVButn->setText("v"); m_gui->letterWButn->setText("w"); m_gui->letterXButn->setText("x");
        m_gui->letterYButn->setText("y"); m_gui->letterZButn->setText("z"); m_capsLock = false;
    } else {
        m_gui->letterAButn->setText("A"); m_gui->letterBButn->setText("B"); m_gui->letterCButn->setText("C"); m_gui->letterDButn->setText("D");
        m_gui->letterEButn->setText("E"); m_gui->letterFButn->setText("F"); m_gui->letterGButn->setText("G"); m_gui->letterHButn->setText("H");
        m_gui->letterIButn->setText("I"); m_gui->letterJButn->setText("J"); m_gui->letterKButn->setText("K"); m_gui->letterLButn->setText("L");
        m_gui->letterMButn->setText("M"); m_gui->letterNButn->setText("N"); m_gui->letterOButn->setText("O"); m_gui->letterPButn->setText("P");
        m_gui->letterQButn->setText("Q"); m_gui->letterRButn->setText("R"); m_gui->letterSButn->setText("S"); m_gui->letterTButn->setText("T");
        m_gui->letterUButn->setText("U"); m_gui->letterVButn->setText("V"); m_gui->letterWButn->setText("W"); m_gui->letterXButn->setText("X");
        m_gui->letterYButn->setText("Y"); m_gui->letterZButn->setText("Z"); m_capsLock = true;
    }
}

void ContactManager::hideFunction()
{
    m_gui->keyboardFrame->hide();
}

void ContactManager::clearFunction()
{
    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->clear();
    if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->clear();
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
