#ifndef SERIALSETTINGS_H
#define SERIALSETTINGS_H

#include <QtSerialPort/QSerialPort>

namespace Ui { class MainWindow; }
class MainWindow;
class QIntValidator;

class SerialSettings : public QObject
{
    Q_OBJECT

signals:
    void serialApplied();

public:
    struct Settings {
        QString name;
        qint32 baudRate;
        QString stringBaudRate;
        QSerialPort::DataBits dataBits;
        QString stringDataBits;
        QSerialPort::Parity parity;
        QString stringParity;
        QSerialPort::StopBits stopBits;
        QString stringStopBits;
        QSerialPort::FlowControl flowControl;
        QString stringFlowControl;
    };

    explicit SerialSettings(QObject* parent = nullptr);
    ~SerialSettings();

    Settings getSettings() const;

private slots:
    void applySettings();
    void checkCustomBaudRate(int idx);
    void checkCustomDevice(int idx);

private:
    Ui::MainWindow* m_gui;
    MainWindow* m_mainWindow = nullptr;
    QIntValidator* m_intValidator = nullptr;
    Settings m_currentSettings;

    void updateSettings();
    void setDefaultPort();
    void addPortInfo();
    void addPortParam();
};

#endif // SERIALSETTINGS_H
