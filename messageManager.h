#ifndef MESSAGEMANAGER_H
#define MESSAGEMANAGER_H

#include <QObject>

namespace Ui { class MainWindow; }
class QTimer;

class MainWindow;
class EncryptTool;
class SerialInterface;
class ProgressBar;

class MessageManager : public QObject
{
    Q_OBJECT

signals:
    void closingUp();

public:
    explicit MessageManager(QObject* parent = nullptr);
    ~MessageManager();

private slots:
    void transmitMessage();
    void groupSelected(int idx);
    void messageDataReceived();
    void messageDataSent();
    void remoteMessageProgress(qreal length, QString type);
    void localMessageProgress(qreal length, QString type);
    void pageChanged();
    void checkMsgReceived();
    void checkMsgSent();

private:
    Ui::MainWindow* m_gui = nullptr;
    MainWindow* m_mainWindow = nullptr;
    EncryptTool* m_encryptTool = nullptr;
    SerialInterface* m_serialInterface = nullptr;
    QTimer* m_timerRec = nullptr;
    QTimer* m_timerSend = nullptr;

    QString m_baseDir;
    QList<ProgressBar*> m_msgRemoteSizeList;
    QList<ProgressBar*> m_msgLocalSizeList;
    bool m_receivingStatus;
    bool m_sendingStatus;

    void updateContactList();
};

#endif // MESSAGEMANAGER_H
