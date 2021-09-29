#ifndef CONTACTMANAGER_H
#define CONTACTMANAGER_H

#include <QObject>

namespace Ui { class MainWindow; }
class QTimer;

class MainWindow;
class EncryptTool;
class ProgressBar;
class SerialInterface;

class ContactManager : public QObject
{
    Q_OBJECT

public:
    explicit ContactManager(QObject* parent = nullptr);
    ~ContactManager();

private slots:
    void acceptContact();
    void deleteContact();
    void cancelContact();

    void groupSelected(int idx);
    void nameSelected(int idx);
    void remoteContactProgress(qreal length, QString type);
    void localContactProgress(qreal length, QString type);
    void contactKeyReceived();
    void contactKeySent();
    void contactKeyError();
    void sendPublicKey();

    void pageChanged();
    void checkKeySent();

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
    void focusFunction(QWidget* old, QWidget* now);
    void capsFunction();
    void hideFunction();
    void clearFunction();

private:
    Ui::MainWindow* m_gui = nullptr;
    MainWindow* m_mainWindow = nullptr;
    EncryptTool* m_encryptTool = nullptr;
    SerialInterface* m_serialInterface = nullptr;
    QTimer* m_timer = nullptr;

    QList<ProgressBar*> m_cntRemoteSizeList;
    QList<ProgressBar*> m_cntLocalSizeList;
    bool m_receivingStatus;
    bool m_sendingStatus;
    bool m_capsLock;

    void updateContactList();
};

#endif // CONTACTMANAGER_H
