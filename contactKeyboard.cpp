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
}

ContactKeyboard::~ContactKeyboard()
{
}

void ContactKeyboard::num0Char()
{
    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("0");
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("0");
}

void ContactKeyboard::num1Char()
{
    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("1");
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("1");
}

void ContactKeyboard::num2Char()
{
    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("2");
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("2");
}

void ContactKeyboard::num3Char()
{
    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("3");
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("3");
}

void ContactKeyboard::num4Char()
{
    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("4");
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("4");
}

void ContactKeyboard::num5Char()
{
    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("5");
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("5");
}

void ContactKeyboard::num6Char()
{
    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("6");
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("6");
}

void ContactKeyboard::num7Char()
{
    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("7");
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("7");
}

void ContactKeyboard::num8Char()
{
    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("8");
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("8");
}

void ContactKeyboard::num9Char()
{
    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("9");
    else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("9");
}

void ContactKeyboard::letterAChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("A");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("A");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("a");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("a");
    }
}

void ContactKeyboard::letterBChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("B");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("B");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("b");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("b");
    }
}

void ContactKeyboard::letterCChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("C");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("C");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("c");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("c");
    }
}

void ContactKeyboard::letterDChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("D");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("D");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("d");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("d");
    }
}

void ContactKeyboard::letterEChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("E");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("E");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("e");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("e");
    }
}

void ContactKeyboard::letterFChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("F");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("F");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("f");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("f");
    }
}

void ContactKeyboard::letterGChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("G");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("G");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("g");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("g");
    }
}

void ContactKeyboard::letterHChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("H");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("H");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("h");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("h");
    }
}

void ContactKeyboard::letterIChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("I");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("I");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("i");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("i");
    }
}

void ContactKeyboard::letterJChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("J");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("J");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("j");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("j");
    }
}

void ContactKeyboard::letterKChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("K");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("K");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("k");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("k");
    }
}

void ContactKeyboard::letterLChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("L");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("L");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("l");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("l");
    }
}

void ContactKeyboard::letterMChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("M");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("M");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("m");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("m");
    }
}

void ContactKeyboard::letterNChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("N");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("N");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("n");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("n");
    }
}

void ContactKeyboard::letterOChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("O");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("O");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("o");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("o");
    }
}

void ContactKeyboard::letterPChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("P");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("P");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("p");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("p");
    }
}

void ContactKeyboard::letterQChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("Q");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("Q");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("q");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("q");
    }
}

void ContactKeyboard::letterRChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("R");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("R");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("r");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("r");
    }
}

void ContactKeyboard::letterSChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("S");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("S");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("s");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("s");
    }
}

void ContactKeyboard::letterTChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("T");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("T");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("t");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("t");
    }
}

void ContactKeyboard::letterUChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("U");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("U");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("u");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("u");
    }
}

void ContactKeyboard::letterVChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("V");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("V");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("v");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("v");
    }
}

void ContactKeyboard::letterWChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("W");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("W");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("w");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("w");
    }
}

void ContactKeyboard::letterXChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("X");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("X");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("x");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("x");
    }
}

void ContactKeyboard::letterYChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("Y");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("Y");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("y");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("y");
    }
}

void ContactKeyboard::letterZChar()
{
    if (m_capsLock) {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("Z");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("Z");
    } else {
        if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->insert("z");
        else if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->insert("z");
    }
}

void ContactKeyboard::letterSpace()
{
    if (m_gui->cntNameLEdit->hasFocus())
        m_gui->cntNameLEdit->insert(" ");
    else if (m_gui->cntGroupLEdit->hasFocus())
        m_gui->cntGroupLEdit->insert(" ");
}

void ContactKeyboard::focusFunction(QWidget*, QWidget* now)
{
    if (now == m_gui->cntNameLEdit || now == m_gui->cntGroupLEdit) m_gui->keyboardFrame->show();
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
    m_gui->keyboardFrame->hide();
}

void ContactKeyboard::clearFunction()
{
    if (m_gui->cntNameLEdit->hasFocus()) m_gui->cntNameLEdit->clear();
    if (m_gui->cntGroupLEdit->hasFocus()) m_gui->cntGroupLEdit->clear();
}

