#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui { class MainWindow; }
class SerialSettings;
class SerialInterface;
class EncryptTool;
class ContactManager;
class ContactKeyboard;
class MessageManager;
class SoundManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    QString getOwnPKeyHash();
    Ui::MainWindow* getGUI();
    EncryptTool* getEncryptTool();
    SerialSettings* getSerialSettings();
    SerialInterface* getSerialInterface();
    ContactManager* getContactManager();
    ContactKeyboard* getContactKeyboard();
    MessageManager* getMessageManager();
    SoundManager* getSoundManager();

private slots:
    void setAudioWidget();
    void setContactsWidget();
    void setMessagesWidget();
    void setSettingsWidget();
    void setContactReceiveWidget();
    void setContactSendWidget();
    void setMessageReceiveWidget();
    void setMessageSendWidget();
    void setSettingSerialWidget();
    void setSettingOffFunction();
    void setSettingAboutWidget();

private:
    Ui::MainWindow* m_gui = nullptr;
    EncryptTool* m_encryptTool = nullptr;
    SerialSettings* m_serialSettings = nullptr;
    SerialInterface* m_serialInterface = nullptr;
    ContactManager* m_contactManager = nullptr;
    ContactKeyboard* m_contactKeyboard = nullptr;
    MessageManager* m_messageManager = nullptr;
    SoundManager* m_soundManager = nullptr;

    QString m_baseDir;
    QString m_pKeyHash;

    void afterInit();
    void loadLocalKeys();
    void generateLocalKeys();
};

#endif // MAINWINDOW_H
