#ifndef CONTACTKEYBOARD_H
#define CONTACTKEYBOARD_H

#include <QObject>

namespace Ui { class MainWindow; }
class MainWindow;

class ContactKeyboard : public QObject
{
    Q_OBJECT

public:
    explicit ContactKeyboard(QObject* parent = nullptr);
    ~ContactKeyboard();

private slots:
    void num0Char();
    void num1Char();
    void num2Char();
    void num3Char();
    void num4Char();
    void num5Char();
    void num6Char();
    void num7Char();
    void num8Char();
    void num9Char();
    void letterAChar();
    void letterBChar();
    void letterCChar();
    void letterDChar();
    void letterEChar();
    void letterFChar();
    void letterGChar();
    void letterHChar();
    void letterIChar();
    void letterJChar();
    void letterKChar();
    void letterLChar();
    void letterMChar();
    void letterNChar();
    void letterOChar();
    void letterPChar();
    void letterQChar();
    void letterRChar();
    void letterSChar();
    void letterTChar();
    void letterUChar();
    void letterVChar();
    void letterWChar();
    void letterXChar();
    void letterYChar();
    void letterZChar();
    void letterSpace();
    void focusFunction(QWidget*, QWidget* now);
    void capsFunction();
    void hideFunction();
    void clearFunction();

private:
    Ui::MainWindow* m_gui = nullptr;
    MainWindow* m_mainWindow = nullptr;

    bool m_capsLock;
};

#endif // CONTACTKEYBOARD_H
