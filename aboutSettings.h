#ifndef ABOUTSETTINGS_H
#define ABOUTSETTINGS_H

#include <QObject>
#include <QProcess>

namespace Ui { class MainWindow; }
class MainWindow;
class EncryptTool;

class AboutSettings : public QObject
{
    Q_OBJECT

public:
    explicit AboutSettings(QObject* parent = nullptr);
    ~AboutSettings();

    QString getOwnPKeyHash();

private slots:
    void applySettings();
    void loadLocalKeys(int, QProcess::ExitStatus);
    void saveLocalKeys(int, QProcess::ExitStatus);

private:
    Ui::MainWindow* m_gui;
    MainWindow* m_mainWindow = nullptr;
    EncryptTool* m_encryptTool = nullptr;
    QProcess* m_process1 = nullptr;
    QProcess* m_process2 = nullptr;

    QString m_baseDir;
    QString m_pKeyHash;

    void generateLocalKeys();
};

#endif // ABOUTSETTINGS_H
