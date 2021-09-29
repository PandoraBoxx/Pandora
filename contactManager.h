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

    void updateContactList();
};

#endif // CONTACTMANAGER_H
