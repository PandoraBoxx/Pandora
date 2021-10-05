/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "serialInterface.h"
#include "mainWindow.h"
#include "serialSettings.h"

#include <QFile>
#include <QDir>
#include <QDebug>

SerialInterface::SerialInterface(QObject* parent) : QObject(parent)
{
    m_mainWindow = reinterpret_cast<MainWindow*>(parent);
    m_serialSettings = m_mainWindow->getSerialSettings();
    m_serialPort = new QSerialPort(this);
    m_timer00 = new QTimer(this);
    m_timer01 = new QTimer(this);

    m_baseDir = QDir::homePath() + "/PandoraContacts";
    connect(this, &SerialInterface::recvKeySignal, this, &SerialInterface::recvKeySMac);
    connect(this, &SerialInterface::recvMsgSignal, this, &SerialInterface::recvMsgSMac);
    connect(m_serialSettings, &SerialSettings::serialApplied, this, &SerialInterface::openSerialPort);
    connect(m_serialPort, &QSerialPort::errorOccurred, this, &SerialInterface::handleError);
    connect(m_serialPort, &QSerialPort::readyRead, this, &SerialInterface::readData);
    connect(m_timer00, &QTimer::timeout, this, &SerialInterface::checkKeySent);
    connect(m_timer01, &QTimer::timeout, this, &SerialInterface::checkMsgSent);
    m_recvKeyState = RKState00;
    m_recvMsgState = RMState00;
    m_lastReminder = 0;
    m_reminderCounter = 0;
    openSerialPort();
}

SerialInterface::~SerialInterface()
{
}

void SerialInterface::setupSerialPort()
{
    closeSerialPort();
}

void SerialInterface::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        qDebug() << "Critical serial port error " << m_serialPort->errorString();
        closeSerialPort();
    }
}

void SerialInterface::openSerialPort()
{
    const SerialSettings::Settings p = m_serialSettings->getSettings();
    m_serialPort->setPortName(p.name);
    m_serialPort->setBaudRate(p.baudRate);
    m_serialPort->setDataBits(p.dataBits);
    m_serialPort->setParity(p.parity);
    m_serialPort->setStopBits(p.stopBits);
    m_serialPort->setFlowControl(p.flowControl);
    if (m_serialPort->open(QIODevice::ReadWrite)) {
        qDebug() << "Connected to " << p.name << " : " << p.stringBaudRate << " " << p.stringDataBits
                 << " " << p.stringParity << " " << p.stringStopBits << " " << p.stringFlowControl;
    } else {
        qDebug() << "Open serial port error " << m_serialPort->errorString();
    }
}

void SerialInterface::closeSerialPort()
{
    if (m_serialPort->isOpen()) m_serialPort->close();
    qDebug() << "Disconnected";
}

qint64 SerialInterface::writeData(const QByteArray &data)
{
    return m_serialPort->write(data);
}

void SerialInterface::readData()
{
    m_loadedData.push_back(m_serialPort->readAll());
    if (m_recvKeyState != RKState00) emit recvKeySignal();
    else if (m_recvMsgState != RMState00) emit recvMsgSignal();
    else m_loadedData.clear();
}

void SerialInterface::stopReceiving()
{
    m_recvKeyState = RKState00;
    m_recvMsgState = RMState00;
    m_loadedData.clear();
}

void SerialInterface::recvKey()
{
    m_recvKeyState = RKState01;
    m_recvMsgState = RMState00;
    m_loadedData.clear();
}

void SerialInterface::recvKeySMac()
{
    int indx;
    QByteArray choppedArray;

    if (m_recvKeyState == RKState00) {
        m_recvKeyState = RKState00;
        return;
    }

    if (m_recvKeyState == RKState01) {
        indx = m_loadedData.indexOf(QByteArray("lengthOfKey:"));
        if (indx == -1) {
            m_recvKeyState = RKState01;
            return;
        } else {
            indx += 12;
            choppedArray = m_loadedData.mid(indx);
            m_loadedData.clear();
            m_loadedData.prepend(choppedArray);
            m_bytesToLoad = 0;
            m_recvKeyState = RKState02;
        }
    }

    if (m_recvKeyState == RKState02) {
        if (m_loadedData.size() <= 3) {
            m_recvKeyState = RKState02;
            return;
        } else {
            qint32* number = reinterpret_cast<qint32*>(m_loadedData.data());
            m_bytesToLoad = *number;
            choppedArray = m_loadedData.mid(4);
            m_loadedData.clear();
            m_loadedData.prepend(choppedArray);
            m_recvKeyState = RKState03;
        }
    }

    if (m_recvKeyState == RKState03) {
        indx = m_loadedData.indexOf(QByteArray("dataOfKey:"));
        if (indx == -1) {
            m_recvKeyState = RKState03;
            return;
        } else {
            indx += 10;
            choppedArray = m_loadedData.mid(indx);
            m_loadedData.clear();
            m_loadedData.prepend(choppedArray);
            m_recvKeyState = RKState04;
        }
    }

    if (m_recvKeyState == RKState04) {
        qreal loaded = static_cast<qreal>(m_loadedData.size());
        qreal percent = loaded / static_cast<qreal>(m_bytesToLoad);
        if (percent > 1.0F) percent = 1.0F;
        emit cntRemotLengthSignal(percent, "Key");
        if (m_loadedData.size() < m_bytesToLoad) {
            m_recvKeyState = RKState04;
            return;
        } else {
            QFile file("/mnt/ramdisk/remotePub.txt");
            file.open(QIODevice::WriteOnly);
            if (!file.isOpen()) {
                qDebug() << "File remotePub.txt open for write failed";
            } else {
                file.write(m_loadedData.data(), m_bytesToLoad);
                file.close();
                emit cntReceivedSignal();
            }
            m_recvKeyState = RKState00;
        }
    }
}

void SerialInterface::sendKey()
{
    QFile fileKey(m_baseDir + "/localPub.txt");
    fileKey.open(QIODevice::ReadOnly);
    if (fileKey.isOpen()) {

        m_localPubcKey = fileKey.readAll();
        qint32 fileSize = m_localPubcKey.size();
        char* numPoint = reinterpret_cast<char*>(&fileSize);

        m_serialPort->write(QByteArray("lengthOfKey:"));
        m_serialPort->write(numPoint, 4);
        m_serialPort->write(QByteArray("dataOfKey:"));
        m_serialPort->write(m_localPubcKey);

        m_lastReminder = 0;
        m_reminderCounter = 0;
        m_timer00->start(100);
        fileKey.close();
    } else {

        qDebug() << "File localPub.txt open for read failed";
        m_localPubcKey.clear();
        emit cntSentErrorSignal();
    }
}

void SerialInterface::checkKeySent()
{
    if (m_lastReminder == m_serialPort->bytesToWrite()) {
        m_reminderCounter++;
        if (m_reminderCounter > 10) {
            m_serialPort->clear(QSerialPort::AllDirections);
            emit cntSentErrorSignal();
            m_timer00->stop();
            return;
        }
    } else {
        m_reminderCounter = 0;
    }
    qreal unwriten = static_cast<qreal>(m_serialPort->bytesToWrite());
    qreal percent = 1.0F - (unwriten / static_cast<qreal>(m_localPubcKey.size()));
    emit cntLocalLengthSignal(percent, "Key");
    if (m_serialPort->bytesToWrite() == 0) {
        emit cntSentSignal();
        m_timer00->stop();
    }
    m_lastReminder = m_serialPort->bytesToWrite();
}

void SerialInterface::recvMessage()
{
    m_recvMsgState = RMState01;
    m_recvKeyState = RKState00;
    m_loadedData.clear();
}

void SerialInterface::recvMsgSMac()
{
    int indx;
    QByteArray choppedArray;

    if (m_recvMsgState == RMState00) {
        m_recvMsgState = RMState00;
        return;
    }

    if (m_recvMsgState == RMState01) {
        indx = m_loadedData.indexOf(QByteArray("lengthOfMsg:"));
        if (indx == -1) {
            m_recvMsgState = RMState01;
            return;
        } else {
            indx += 12;
            choppedArray = m_loadedData.mid(indx);
            m_loadedData.clear();
            m_loadedData.prepend(choppedArray);
            m_bytesToLoad = 0;
            m_recvMsgState = RMState02;
        }
    }

    if (m_recvMsgState == RMState02) {
        if (m_loadedData.size() <= 3) {
            m_recvMsgState = RMState02;
            return;
        } else {
            qint32* number = reinterpret_cast<qint32*>(m_loadedData.data());
            m_bytesToLoad = *number;
            choppedArray = m_loadedData.mid(4);
            m_loadedData.clear();
            m_loadedData.prepend(choppedArray);
            m_recvMsgState = RMState03;
        }
    }

    if (m_recvMsgState == RMState03) {
        indx = m_loadedData.indexOf(QByteArray("dataOfMsg:"));
        if (indx == -1) {
            m_recvMsgState = RMState03;
            return;
        } else {
            indx += 10;
            choppedArray = m_loadedData.mid(indx);
            m_loadedData.clear();
            m_loadedData.prepend(choppedArray);
            m_recvMsgState = RMState04;
        }
    }

    if (m_recvMsgState == RMState04) {
        qreal loaded = static_cast<qreal>(m_loadedData.size());
        qreal percent = loaded / static_cast<qreal>(m_bytesToLoad);
        if (percent > 1.0F) percent = 1.0F;
        emit msgRemoteLengthSignal(percent, "Message");
        if (m_loadedData.size() < m_bytesToLoad) {
            m_recvMsgState = RMState04;
            return;
        } else {
            QFile file("/mnt/ramdisk/audioRemote.enc");
            file.open(QIODevice::WriteOnly);
            if (!file.isOpen()) {
                qDebug() << "File audioRemote.enc open for write failed";
            } else {
                file.write(m_loadedData.data(), m_bytesToLoad);
                file.close();
                emit msgReceivedSignal();
            }
            m_recvMsgState = RMState00;
        }
    }
}

void SerialInterface::sendMessage()
{
    QFile fileKey("/mnt/ramdisk/audioLocal.enc");
    fileKey.open(QIODevice::ReadOnly);
    if (fileKey.isOpen()) {

        m_localEncdMsg = fileKey.readAll();
        qint32 fileSize = m_localEncdMsg.size();
        char* numPoint = reinterpret_cast<char*>(&fileSize);

        m_serialPort->write(QByteArray("lengthOfMsg:"));
        m_serialPort->write(numPoint, 4);
        m_serialPort->write(QByteArray("dataOfMsg:"));
        m_serialPort->write(m_localEncdMsg);

        m_lastReminder = 0;
        m_reminderCounter = 0;
        fileKey.close();
        m_timer01->start(100);
    } else {

        qDebug() << "File audioLocal.enc open for read failed";
        m_localEncdMsg.clear();
        emit msgSentErrorSignal();
    }
}

void SerialInterface::checkMsgSent()
{
    if (m_lastReminder == m_serialPort->bytesToWrite()) {
        m_reminderCounter++;
        if (m_reminderCounter > 10) {
            m_serialPort->clear(QSerialPort::AllDirections);
            emit msgSentErrorSignal();
            m_timer01->stop();
            return;
        }
    } else {
        m_reminderCounter = 0;
    }
    qreal unwriten = static_cast<qreal>(m_serialPort->bytesToWrite());
    qreal percent = 1.0F - (unwriten / static_cast<qreal>(m_localEncdMsg.size()));
    emit msgLocalLengthSignal(percent, "Msg");
    if (m_serialPort->bytesToWrite() == 0) {
        emit msgSentSignal();
        m_timer01->stop();
    }
    m_lastReminder = m_serialPort->bytesToWrite();
}
