#include "contactKeyboard.h"
#include "mainWindow.h"
#include "ui_mainWindow.h"

ContactKeyboard::ContactKeyboard(QObject* parent) : QObject(parent)
{
    m_mainWindow = reinterpret_cast<MainWindow*>(parent);
    m_gui = m_mainWindow->getGUI();
    m_capsLock = true;

    connect(m_gui->num0Butn, &QPushButton::clicked, this, &ContactKeyboard::num0Char);
    connect(m_gui->num1Butn, &QPushButton::clicked, this, &ContactKeyboard::num1Char);
    connect(m_gui->num2Butn, &QPushButton::clicked, this, &ContactKeyboard::num2Char);
    connect(m_gui->num3Butn, &QPushButton::clicked, this, &ContactKeyboard::num3Char);
    connect(m_gui->num4Butn, &QPushButton::clicked, this, &ContactKeyboard::num4Char);
    connect(m_gui->num5Butn, &QPushButton::clicked, this, &ContactKeyboard::num5Char);
    connect(m_gui->num6Butn, &QPushButton::clicked, this, &ContactKeyboard::num6Char);
    connect(m_gui->num7Butn, &QPushButton::clicked, this, &ContactKeyboard::num7Char);
    connect(m_gui->num8Butn, &QPushButton::clicked, this, &ContactKeyboard::num8Char);
    connect(m_gui->num9Butn, &QPushButton::clicked, this, &ContactKeyboard::num9Char);
    connect(m_gui->letterAButn, &QPushButton::clicked, this, &ContactKeyboard::letterAChar);
    connect(m_gui->letterBButn, &QPushButton::clicked, this, &ContactKeyboard::letterBChar);
    connect(m_gui->letterCButn, &QPushButton::clicked, this, &ContactKeyboard::letterCChar);
    connect(m_gui->letterDButn, &QPushButton::clicked, this, &ContactKeyboard::letterDChar);
    connect(m_gui->letterEButn, &QPushButton::clicked, this, &ContactKeyboard::letterEChar);
    connect(m_gui->letterFButn, &QPushButton::clicked, this, &ContactKeyboard::letterFChar);
    connect(m_gui->letterGButn, &QPushButton::clicked, this, &ContactKeyboard::letterGChar);
    connect(m_gui->letterHButn, &QPushButton::clicked, this, &ContactKeyboard::letterHChar);
    connect(m_gui->letterIButn, &QPushButton::clicked, this, &ContactKeyboard::letterIChar);
    connect(m_gui->letterJButn, &QPushButton::clicked, this, &ContactKeyboard::letterJChar);
    connect(m_gui->letterKButn, &QPushButton::clicked, this, &ContactKeyboard::letterKChar);
    connect(m_gui->letterLButn, &QPushButton::clicked, this, &ContactKeyboard::letterLChar);
    connect(m_gui->letterMButn, &QPushButton::clicked, this, &ContactKeyboard::letterMChar);
    connect(m_gui->letterNButn, &QPushButton::clicked, this, &ContactKeyboard::letterNChar);
    connect(m_gui->letterOButn, &QPushButton::clicked, this, &ContactKeyboard::letterOChar);
    connect(m_gui->letterPButn, &QPushButton::clicked, this, &ContactKeyboard::letterPChar);
    connect(m_gui->letterQButn, &QPushButton::clicked, this, &ContactKeyboard::letterQChar);
    connect(m_gui->letterRButn, &QPushButton::clicked, this, &ContactKeyboard::letterRChar);
    connect(m_gui->letterSButn, &QPushButton::clicked, this, &ContactKeyboard::letterSChar);
    connect(m_gui->letterTButn, &QPushButton::clicked, this, &ContactKeyboard::letterTChar);
    connect(m_gui->letterUButn, &QPushButton::clicked, this, &ContactKeyboard::letterUChar);
    connect(m_gui->letterVButn, &QPushButton::clicked, this, &ContactKeyboard::letterVChar);
    connect(m_gui->letterWButn, &QPushButton::clicked, this, &ContactKeyboard::letterWChar);
    connect(m_gui->letterXButn, &QPushButton::clicked, this, &ContactKeyboard::letterXChar);
    connect(m_gui->letterYButn, &QPushButton::clicked, this, &ContactKeyboard::letterYChar);
    connect(m_gui->letterZButn, &QPushButton::clicked, this, &ContactKeyboard::letterZChar);
    connect(m_gui->spaceButn, &QPushButton::clicked, this, &ContactKeyboard::letterSpace);
    connect(qApp, &QApplication::focusChanged, this, &ContactKeyboard::focusFunction);
    connect(m_gui->capsButn, &QPushButton::clicked, this, &ContactKeyboard::capsFunction);
    connect(m_gui->hideButn, &QPushButton::clicked, this, &ContactKeyboard::hideFunction);
    connect(m_gui->clearButn, &QPushButton::clicked, this, &ContactKeyboard::clearFunction);

    hideFunction();
}

ContactKeyboard::~ContactKeyboard()
{
}

void ContactKeyboard::disconnectQapp()
{
    disconnect(qApp, &QApplication::focusChanged, this, &ContactKeyboard::focusFunction);
}

void ContactKeyboard::num0Char()
{
    QString number = "0";

    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert(number);
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert(number);
    else if (m_gui->passwordLEdit->hasFocus()) m_gui->passwordLEdit->insert(number);
}

void ContactKeyboard::num1Char()
{
    QString number = "1";

    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert(number);
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert(number);
    else if (m_gui->passwordLEdit->hasFocus()) m_gui->passwordLEdit->insert(number);
}

void ContactKeyboard::num2Char()
{
    QString number = "2";

    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert(number);
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert(number);
    else if (m_gui->passwordLEdit->hasFocus()) m_gui->passwordLEdit->insert(number);
}

void ContactKeyboard::num3Char()
{
    QString number = "3";

    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert(number);
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert(number);
    else if (m_gui->passwordLEdit->hasFocus()) m_gui->passwordLEdit->insert(number);
}

void ContactKeyboard::num4Char()
{
    QString number = "4";

    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert(number);
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert(number);
    else if (m_gui->passwordLEdit->hasFocus()) m_gui->passwordLEdit->insert(number);
}

void ContactKeyboard::num5Char()
{
    QString number = "5";

    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert(number);
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert(number);
    else if (m_gui->passwordLEdit->hasFocus()) m_gui->passwordLEdit->insert(number);
}

void ContactKeyboard::num6Char()
{
    QString number = "6";

    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert(number);
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert(number);
    else if (m_gui->passwordLEdit->hasFocus()) m_gui->passwordLEdit->insert(number);
}

void ContactKeyboard::num7Char()
{
    QString number = "7";

    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert(number);
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert(number);
    else if (m_gui->passwordLEdit->hasFocus()) m_gui->passwordLEdit->insert(number);
}

void ContactKeyboard::num8Char()
{
    QString number = "8";

    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert(number);
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert(number);
    else if (m_gui->passwordLEdit->hasFocus()) m_gui->passwordLEdit->insert(number);
}

void ContactKeyboard::num9Char()
{
    QString number = "9";

    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert(number);
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert(number);
    else if (m_gui->passwordLEdit->hasFocus()) m_gui->passwordLEdit->insert(number);
}

void ContactKeyboard::letterAChar()
{
    QString letter = "a";
    if (m_capsLock) letter = "A";

    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert(letter);
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert(letter);
    else if (m_gui->passwordLEdit->hasFocus()) m_gui->passwordLEdit->insert(letter);
}

void ContactKeyboard::letterBChar()
{
    QString letter = "b";
    if (m_capsLock) letter = "B";

    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert(letter);
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert(letter);
    else if (m_gui->passwordLEdit->hasFocus()) m_gui->passwordLEdit->insert(letter);
}

void ContactKeyboard::letterCChar()
{
    QString letter = "c";
    if (m_capsLock) letter = "C";

    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert(letter);
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert(letter);
    else if (m_gui->passwordLEdit->hasFocus()) m_gui->passwordLEdit->insert(letter);
}

void ContactKeyboard::letterDChar()
{
    QString letter = "d";
    if (m_capsLock) letter = "D";

    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert(letter);
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert(letter);
    else if (m_gui->passwordLEdit->hasFocus()) m_gui->passwordLEdit->insert(letter);
}

void ContactKeyboard::letterEChar()
{
    QString letter = "e";
    if (m_capsLock) letter = "E";

    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert(letter);
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert(letter);
    else if (m_gui->passwordLEdit->hasFocus()) m_gui->passwordLEdit->insert(letter);
}

void ContactKeyboard::letterFChar()
{
    QString letter = "f";
    if (m_capsLock) letter = "F";

    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert(letter);
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert(letter);
    else if (m_gui->passwordLEdit->hasFocus()) m_gui->passwordLEdit->insert(letter);
}

void ContactKeyboard::letterGChar()
{
    QString letter = "g";
    if (m_capsLock) letter = "G";

    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert(letter);
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert(letter);
    else if (m_gui->passwordLEdit->hasFocus()) m_gui->passwordLEdit->insert(letter);
}

void ContactKeyboard::letterHChar()
{
    QString letter = "h";
    if (m_capsLock) letter = "H";

    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert(letter);
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert(letter);
    else if (m_gui->passwordLEdit->hasFocus()) m_gui->passwordLEdit->insert(letter);
}

void ContactKeyboard::letterIChar()
{
    QString letter = "i";
    if (m_capsLock) letter = "I";

    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert(letter);
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert(letter);
    else if (m_gui->passwordLEdit->hasFocus()) m_gui->passwordLEdit->insert(letter);
}

void ContactKeyboard::letterJChar()
{
    QString letter = "j";
    if (m_capsLock) letter = "J";

    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert(letter);
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert(letter);
    else if (m_gui->passwordLEdit->hasFocus()) m_gui->passwordLEdit->insert(letter);
}

void ContactKeyboard::letterKChar()
{
    QString letter = "k";
    if (m_capsLock) letter = "K";

    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert(letter);
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert(letter);
    else if (m_gui->passwordLEdit->hasFocus()) m_gui->passwordLEdit->insert(letter);
}

void ContactKeyboard::letterLChar()
{
    QString letter = "l";
    if (m_capsLock) letter = "L";

    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert(letter);
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert(letter);
    else if (m_gui->passwordLEdit->hasFocus()) m_gui->passwordLEdit->insert(letter);
}

void ContactKeyboard::letterMChar()
{
    QString letter = "m";
    if (m_capsLock) letter = "M";

    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert(letter);
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert(letter);
    else if (m_gui->passwordLEdit->hasFocus()) m_gui->passwordLEdit->insert(letter);
}

void ContactKeyboard::letterNChar()
{
    QString letter = "n";
    if (m_capsLock) letter = "N";

    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert(letter);
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert(letter);
    else if (m_gui->passwordLEdit->hasFocus()) m_gui->passwordLEdit->insert(letter);
}

void ContactKeyboard::letterOChar()
{
    QString letter = "o";
    if (m_capsLock) letter = "O";

    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert(letter);
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert(letter);
    else if (m_gui->passwordLEdit->hasFocus()) m_gui->passwordLEdit->insert(letter);
}

void ContactKeyboard::letterPChar()
{
    QString letter = "p";
    if (m_capsLock) letter = "P";

    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert(letter);
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert(letter);
    else if (m_gui->passwordLEdit->hasFocus()) m_gui->passwordLEdit->insert(letter);
}

void ContactKeyboard::letterQChar()
{
    QString letter = "q";
    if (m_capsLock) letter = "Q";

    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert(letter);
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert(letter);
    else if (m_gui->passwordLEdit->hasFocus()) m_gui->passwordLEdit->insert(letter);
}

void ContactKeyboard::letterRChar()
{
    QString letter = "r";
    if (m_capsLock) letter = "R";

    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert(letter);
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert(letter);
    else if (m_gui->passwordLEdit->hasFocus()) m_gui->passwordLEdit->insert(letter);
}

void ContactKeyboard::letterSChar()
{
    QString letter = "s";
    if (m_capsLock) letter = "S";

    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert(letter);
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert(letter);
    else if (m_gui->passwordLEdit->hasFocus()) m_gui->passwordLEdit->insert(letter);
}

void ContactKeyboard::letterTChar()
{
    QString letter = "t";
    if (m_capsLock) letter = "T";

    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert(letter);
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert(letter);
    else if (m_gui->passwordLEdit->hasFocus()) m_gui->passwordLEdit->insert(letter);
}

void ContactKeyboard::letterUChar()
{
    QString letter = "u";
    if (m_capsLock) letter = "U";

    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert(letter);
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert(letter);
    else if (m_gui->passwordLEdit->hasFocus()) m_gui->passwordLEdit->insert(letter);
}

void ContactKeyboard::letterVChar()
{
    QString letter = "v";
    if (m_capsLock) letter = "V";

    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert(letter);
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert(letter);
    else if (m_gui->passwordLEdit->hasFocus()) m_gui->passwordLEdit->insert(letter);
}

void ContactKeyboard::letterWChar()
{
    QString letter = "w";
    if (m_capsLock) letter = "W";

    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert(letter);
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert(letter);
    else if (m_gui->passwordLEdit->hasFocus()) m_gui->passwordLEdit->insert(letter);
}

void ContactKeyboard::letterXChar()
{
    QString letter = "x";
    if (m_capsLock) letter = "X";

    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert(letter);
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert(letter);
    else if (m_gui->passwordLEdit->hasFocus()) m_gui->passwordLEdit->insert(letter);
}

void ContactKeyboard::letterYChar()
{
    QString letter = "y";
    if (m_capsLock) letter = "Y";

    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert(letter);
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert(letter);
    else if (m_gui->passwordLEdit->hasFocus()) m_gui->passwordLEdit->insert(letter);
}

void ContactKeyboard::letterZChar()
{
    QString letter = "z";
    if (m_capsLock) letter = "Z";

    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert(letter);
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert(letter);
    else if (m_gui->passwordLEdit->hasFocus()) m_gui->passwordLEdit->insert(letter);
}

void ContactKeyboard::letterSpace()
{
    QString letter = " ";

    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert(letter);
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert(letter);
    else if (m_gui->passwordLEdit->hasFocus()) m_gui->passwordLEdit->insert(letter);
}

void ContactKeyboard::focusFunction(QWidget*, QWidget* now)
{
    if (now == m_gui->cntNameLEdit || now == m_gui->cntGroupLEdit || now == m_gui->passwordLEdit) m_gui->keyboardFrame->show();
}

void ContactKeyboard::capsFunction()
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

void ContactKeyboard::hideFunction()
{
    if (!m_gui->passwordLEdit->hasFocus()) m_gui->keyboardFrame->hide();
}

void ContactKeyboard::clearFunction()
{
    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->clear();
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->clear();
    else if (m_gui->passwordLEdit->hasFocus()) m_gui->passwordLEdit->clear();
}

