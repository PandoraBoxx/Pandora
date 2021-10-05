#include "soundManager.h"
#include "mainWindow.h"
#include "ui_mainWindow.h"
#include "progressBar.h"

#include <QAudioProbe>
#include <QAudioRecorder>
#include <QFileDialog>
#include <QProcess>

static qreal getPeakValue(const QAudioFormat &format);
static QVector<qreal> getBufferLevels(const QAudioBuffer &buffer);
template <class T> static QVector<qreal> getBufferLevels(const T *buffer, int frames, int channels);

SoundManager::SoundManager(QObject* parent) : QObject(parent)
{
    m_mainWindow = reinterpret_cast<MainWindow*>(parent);
    m_gui = m_mainWindow->getGUI();
    m_ramdiskDir = m_mainWindow->getRamdiskDir();

    m_qAudioRecorder = new QAudioRecorder(this);
    m_process = new QProcess(this);
    m_probe = new QAudioProbe(this);

    ProgressBar* vumeter = new ProgressBar(m_gui->audioPage);
    m_vumeterList.append(vumeter);
    m_gui->audLevelHLayout->addWidget(vumeter);

    connect(m_probe, &QAudioProbe::audioBufferProbed, this, &SoundManager::processBuffer);
    m_probe->setSource(m_qAudioRecorder);

    m_qAudioRecorder->setOutputLocation(QUrl::fromLocalFile(m_ramdiskDir + "/audioLocal.org"));

    connect(m_qAudioRecorder, &QAudioRecorder::durationChanged, this, &SoundManager::updateProgress);
    connect(m_qAudioRecorder, &QAudioRecorder::statusChanged, this, &SoundManager::updateStatus);
    connect(m_qAudioRecorder, &QAudioRecorder::stateChanged, this, &SoundManager::onStateChanged);
    connect(m_qAudioRecorder, QOverload<QMediaRecorder::Error>::of(&QAudioRecorder::error), this, &SoundManager::displayErrorMessage);

    connect(m_gui->audRecordButn, &QPushButton::clicked, this, &SoundManager::toggleRecord);
    connect(m_gui->audPauseButn, &QPushButton::clicked, this, &SoundManager::togglePause);
    connect(m_gui->audPlayButn, &QPushButton::clicked, this, &SoundManager::playAudio);
}

SoundManager::~SoundManager()
{
}

qreal getPeakValue(const QAudioFormat& format)
{
    /* This function returns the maximum possible sample value for a given audio format */
    /* Note: Only the most common sample formats are supported */
    if (!format.isValid())
        return qreal(0);

    if (format.codec() != "audio/pcm")
        return qreal(0);

    switch (format.sampleType()) {
    case QAudioFormat::Unknown:
        break;
    case QAudioFormat::Float:
        if (format.sampleSize() != 32) // other sample formats are not supported
            return qreal(0);
        return qreal(1.00003);
    case QAudioFormat::SignedInt:
        if (format.sampleSize() == 32)
            return qreal(INT_MAX);
        if (format.sampleSize() == 16)
            return qreal(SHRT_MAX);
        if (format.sampleSize() == 8)
            return qreal(CHAR_MAX);
        break;
    case QAudioFormat::UnSignedInt:
        if (format.sampleSize() == 32)
            return qreal(UINT_MAX);
        if (format.sampleSize() == 16)
            return qreal(USHRT_MAX);
        if (format.sampleSize() == 8)
            return qreal(UCHAR_MAX);
        break;
    }

    return qreal(0);
}

QVector<qreal> getBufferLevels(const QAudioBuffer& buffer)
{
    /* Here we return the audio level for each channel */
    QVector<qreal> values;

    if (!buffer.format().isValid() || buffer.format().byteOrder() != QAudioFormat::LittleEndian)
        return values;

    if (buffer.format().codec() != "audio/pcm")
        return values;

    int channelCount = buffer.format().channelCount();
    values.fill(0, channelCount);
    qreal peak_value = getPeakValue(buffer.format());
    if (qFuzzyCompare(peak_value, qreal(0)))
        return values;

    switch (buffer.format().sampleType()) {
    case QAudioFormat::Unknown:
    case QAudioFormat::UnSignedInt:
        if (buffer.format().sampleSize() == 32)
            values = getBufferLevels(buffer.constData<quint32>(), buffer.frameCount(), channelCount);
        if (buffer.format().sampleSize() == 16)
            values = getBufferLevels(buffer.constData<quint16>(), buffer.frameCount(), channelCount);
        if (buffer.format().sampleSize() == 8)
            values = getBufferLevels(buffer.constData<quint8>(), buffer.frameCount(), channelCount);
        for (int i = 0; i < values.size(); ++i)
            values[i] = qAbs(values.at(i) - peak_value / 2) / (peak_value / 2);
        break;
    case QAudioFormat::Float:
        if (buffer.format().sampleSize() == 32) {
            values = getBufferLevels(buffer.constData<float>(), buffer.frameCount(), channelCount);
            for (int i = 0; i < values.size(); ++i)
                values[i] /= peak_value;
        }
        break;
    case QAudioFormat::SignedInt:
        if (buffer.format().sampleSize() == 32)
            values = getBufferLevels(buffer.constData<qint32>(), buffer.frameCount(), channelCount);
        if (buffer.format().sampleSize() == 16)
            values = getBufferLevels(buffer.constData<qint16>(), buffer.frameCount(), channelCount);
        if (buffer.format().sampleSize() == 8)
            values = getBufferLevels(buffer.constData<qint8>(), buffer.frameCount(), channelCount);
        for (int i = 0; i < values.size(); ++i)
            values[i] /= peak_value;
        break;
    }

    return values;
}

template <class T> QVector<qreal> getBufferLevels(const T *buffer, int frames, int channels)
{
    QVector<qreal> max_values;
    max_values.fill(0, channels);

    for (int i = 0; i < frames; ++i) {
        for (int j = 0; j < channels; ++j) {
            qreal value = qAbs(qreal(buffer[i * channels + j]));
            if (value > max_values.at(j))
                max_values.replace(j, value);
        }
    }

    return max_values;
}

void SoundManager::updateProgress(qint64 duration)
{
    if (m_qAudioRecorder->error() != QMediaRecorder::NoError || duration < 2000) return;
    m_gui->messageLabel->setText(tr("Recorded %1 sec").arg(duration / 1000));
}

void SoundManager::updateStatus(QMediaRecorder::Status status)
{
    QString statusMessage;

    switch (status) {
    case QMediaRecorder::RecordingStatus:
        statusMessage = tr("Recording to %1").arg(m_qAudioRecorder->actualLocation().toString());
        break;
    case QMediaRecorder::PausedStatus:
        clearAudioLevels();
        statusMessage = tr("Paused");
        break;
    case QMediaRecorder::UnloadedStatus:
    case QMediaRecorder::LoadedStatus:
        clearAudioLevels();
        statusMessage = tr("Stopped");
        break;
    default:
        break;
    }

    if (m_qAudioRecorder->error() == QMediaRecorder::NoError)
        m_gui->messageLabel->setText(statusMessage);
}

void SoundManager::onStateChanged(QMediaRecorder::State state)
{
    switch (state) {
    case QMediaRecorder::RecordingState:
        m_gui->audRecordButn->setText(tr("Stop"));
        m_gui->audPauseButn->setText(tr("Pause"));
        break;
    case QMediaRecorder::PausedState:
        m_gui->audRecordButn->setText(tr("Stop"));
        m_gui->audPauseButn->setText(tr("Resume"));
        break;
    case QMediaRecorder::StoppedState:
        m_gui->audRecordButn->setText(tr("Record"));
        m_gui->audPauseButn->setText(tr("Pause"));
        break;
    }

    m_gui->audPauseButn->setEnabled(m_qAudioRecorder->state() != QMediaRecorder::StoppedState);
}

void SoundManager::toggleRecord()
{
    if (m_qAudioRecorder->state() == QMediaRecorder::StoppedState) {
        m_qAudioRecorder->setAudioInput("");

        QAudioEncoderSettings settings;
        settings.setCodec("");
        settings.setSampleRate(0);
        settings.setBitRate(0);
        settings.setChannelCount(1);
        settings.setQuality(QMultimedia::EncodingQuality(2));
        settings.setEncodingMode(QMultimedia::ConstantQualityEncoding);

        QString container = "";

        m_qAudioRecorder->setEncodingSettings(settings, QVideoEncoderSettings(), container);
        m_qAudioRecorder->record();
    }
    else {
        m_qAudioRecorder->stop();
    }
}

void SoundManager::togglePause()
{
    if (m_qAudioRecorder->state() != QMediaRecorder::PausedState)
        m_qAudioRecorder->pause();
    else
        m_qAudioRecorder->record();
}

void SoundManager::displayErrorMessage()
{
    m_gui->messageLabel->setText(m_qAudioRecorder->errorString());
}

void SoundManager::clearAudioLevels()
{
    for (int i = 0; i < m_vumeterList.size(); ++i)
        m_vumeterList.at(i)->setLevel(0);
}

void SoundManager::processBuffer(const QAudioBuffer& buffer)
{
/*    if (m_vumeterList.count() != buffer.format().channelCount()) {
        qDeleteAll(m_vumeterList);
        m_vumeterList.clear();
        for (int i = 0; i < buffer.format().channelCount(); ++i) {
            ProgressBar* level = new ProgressBar(m_gui->audioPage);
            m_vumeterList.append(level);
            m_gui->audLevelHLayout->addWidget(level);
        }
    }

    QVector<qreal> levels = getBufferLevels(buffer);
    for (int i = 0; i < levels.count(); ++i)
        m_vumeterList.at(i)->setLevel(levels.at(i)); */

    QVector<qreal> levels = getBufferLevels(buffer);
    if (levels.count() > 0) m_vumeterList.at(0)->setLevel(levels.at(0));
}

void SoundManager::playAudio()
{
    QString fname;
    if (m_gui->audSourceCBox->isChecked()) fname = m_ramdiskDir + "/audioRemote.dec";
    else fname = m_ramdiskDir + "/audioLocal.org";
    QString file = "mplayer " + fname;
    m_process->start(file);
}
