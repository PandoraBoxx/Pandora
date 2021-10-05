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
class AboutSettings;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    Ui::MainWindow* getGUI();
    EncryptTool* getEncryptTool();
    SerialSettings* getSerialSettings();
    SerialInterface* getSerialInterface();
    ContactManager* getContactManager();
    ContactKeyboard* getContactKeyboard();
    MessageManager* getMessageManager();
    SoundManager* getSoundManager();
    AboutSettings* getAboutSettings();

public slots:
    void setSettingOffFunction();

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
    AboutSettings* m_aboutSettings = nullptr;

    void afterInit();
};

#endif // MAINWINDOW_H
