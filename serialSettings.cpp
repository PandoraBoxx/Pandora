#include "serialSettings.h"
#include "mainWindow.h"
#include "ui_mainWindow.h"

#include <QtSerialPort/QSerialPortInfo>
#include <QIntValidator>

static const char blankString[] = QT_TRANSLATE_NOOP("SerialSettings", "N/A");

SerialSettings::SerialSettings(QObject* parent) : QObject(parent)
{
    m_mainWindow = reinterpret_cast<MainWindow*>(parent);
    m_gui = m_mainWindow->getGUI();

    m_intValidator = new QIntValidator(0, 4000000, this);
    m_gui->setBaudRateCbBox->setInsertPolicy(QComboBox::NoInsert);
    connect(m_gui->setApplyButn, &QPushButton::clicked, this, &SerialSettings::applySettings);
    connect(m_gui->setBaudRateCbBox,  QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SerialSettings::checkCustomBaudRate);
    connect(m_gui->setSerialPortInfoLBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SerialSettings::checkCustomDevice);
    addPortParam();
    addPortInfo();
    setDefaultPort();
}

SerialSettings::~SerialSettings()
{
}

SerialSettings::Settings SerialSettings::getSettings() const
{
    return m_currentSettings;
}

void SerialSettings::applySettings()
{
    updateSettings();
    emit serialApplied();
}

void SerialSettings::checkCustomBaudRate(int idx)
{
    const bool isCustomBaudRate = !m_gui->setBaudRateCbBox->itemData(idx).isValid();
    m_gui->setBaudRateCbBox->setEditable(isCustomBaudRate);
    if (isCustomBaudRate) {
        m_gui->setBaudRateCbBox->clearEditText();
        QLineEdit* edit = m_gui->setBaudRateCbBox->lineEdit();
        edit->setValidator(m_intValidator);
    }
}

void SerialSettings::checkCustomDevice(int idx)
{
    const bool isCustomPath = !m_gui->setSerialPortInfoLBox->itemData(idx).isValid();
    m_gui->setSerialPortInfoLBox->setEditable(isCustomPath);
    if (isCustomPath) m_gui->setSerialPortInfoLBox->clearEditText();
}

void SerialSettings::addPortParam()
{
    m_gui->setBaudRateCbBox->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    m_gui->setBaudRateCbBox->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    m_gui->setBaudRateCbBox->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    m_gui->setBaudRateCbBox->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    m_gui->setBaudRateCbBox->addItem(tr("Custom"));
    m_gui->setBaudRateCbBox->setCurrentIndex(3);

    m_gui->setDataBitsCbBox->addItem(QStringLiteral("5"), QSerialPort::Data5);
    m_gui->setDataBitsCbBox->addItem(QStringLiteral("6"), QSerialPort::Data6);
    m_gui->setDataBitsCbBox->addItem(QStringLiteral("7"), QSerialPort::Data7);
    m_gui->setDataBitsCbBox->addItem(QStringLiteral("8"), QSerialPort::Data8);
    m_gui->setDataBitsCbBox->setCurrentIndex(3);

    m_gui->setParityCbBox->addItem(tr("None"), QSerialPort::NoParity);
    m_gui->setParityCbBox->addItem(tr("Even"), QSerialPort::EvenParity);
    m_gui->setParityCbBox->addItem(tr("Odd"), QSerialPort::OddParity);
    m_gui->setParityCbBox->addItem(tr("Mark"), QSerialPort::MarkParity);
    m_gui->setParityCbBox->addItem(tr("Space"), QSerialPort::SpaceParity);
    m_gui->setParityCbBox->setCurrentIndex(0);

    m_gui->setStopBitsCbBox->addItem(QStringLiteral("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
    m_gui->setStopBitsCbBox->addItem(tr("1.5"), QSerialPort::OneAndHalfStop);
#endif
    m_gui->setStopBitsCbBox->addItem(QStringLiteral("2"), QSerialPort::TwoStop);
    m_gui->setStopBitsCbBox->setCurrentIndex(0);

    m_gui->setFlowControlCbBox->addItem(tr("None"), QSerialPort::NoFlowControl);
    m_gui->setFlowControlCbBox->addItem(tr("RTS/CTS"), QSerialPort::HardwareControl);
    m_gui->setFlowControlCbBox->addItem(tr("XON/XOFF"), QSerialPort::SoftwareControl);
    m_gui->setFlowControlCbBox->setCurrentIndex(0);
}

void SerialSettings::addPortInfo()
{
    m_gui->setSerialPortInfoLBox->clear();
    QString description;
    QString manufacturer;
    QString serialNumber;
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        QStringList list;
        description = info.description();
        manufacturer = info.manufacturer();
        serialNumber = info.serialNumber();
        list << info.portName()
             << (!description.isEmpty() ? description : blankString)
             << (!manufacturer.isEmpty() ? manufacturer : blankString)
             << (!serialNumber.isEmpty() ? serialNumber : blankString)
             << info.systemLocation()
             << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : blankString)
             << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : blankString);

        m_gui->setSerialPortInfoLBox->addItem(list.first(), list);
    }

    m_gui->setSerialPortInfoLBox->addItem(tr("Custom"));
}

void SerialSettings::setDefaultPort()
{
    int indx = m_gui->setSerialPortInfoLBox->findText("AMA0", Qt::MatchContains);
    if (indx != -1) m_gui->setSerialPortInfoLBox->setCurrentIndex(indx);
    updateSettings();
}

void SerialSettings::updateSettings()
{
    m_currentSettings.name = m_gui->setSerialPortInfoLBox->currentText();

    if (m_gui->setBaudRateCbBox->currentIndex() == 4) {
        m_currentSettings.baudRate = m_gui->setBaudRateCbBox->currentText().toInt();
    } else {
        m_currentSettings.baudRate = static_cast<QSerialPort::BaudRate>(m_gui->setBaudRateCbBox->itemData(m_gui->setBaudRateCbBox->currentIndex()).toInt());
    }
    m_currentSettings.stringBaudRate = QString::number(m_currentSettings.baudRate);

    m_currentSettings.dataBits = static_cast<QSerialPort::DataBits>(m_gui->setDataBitsCbBox->itemData(m_gui->setDataBitsCbBox->currentIndex()).toInt());
    m_currentSettings.stringDataBits = m_gui->setDataBitsCbBox->currentText();

    m_currentSettings.parity = static_cast<QSerialPort::Parity>(m_gui->setParityCbBox->itemData(m_gui->setParityCbBox->currentIndex()).toInt());
    m_currentSettings.stringParity = m_gui->setParityCbBox->currentText();

    m_currentSettings.stopBits = static_cast<QSerialPort::StopBits>(m_gui->setStopBitsCbBox->itemData(m_gui->setStopBitsCbBox->currentIndex()).toInt());
    m_currentSettings.stringStopBits = m_gui->setStopBitsCbBox->currentText();

    m_currentSettings.flowControl = static_cast<QSerialPort::FlowControl>(m_gui->setFlowControlCbBox->itemData(m_gui->setFlowControlCbBox->currentIndex()).toInt());
    m_currentSettings.stringFlowControl = m_gui->setFlowControlCbBox->currentText();
}
