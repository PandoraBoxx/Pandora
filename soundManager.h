#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <QMediaRecorder>

namespace Ui { class MainWindow; }
class QAudioRecorder;
class QAudioProbe;
class QAudioBuffer;
class QProcess;

class MainWindow;
class ProgressBar;

class SoundManager : public QObject
{
    Q_OBJECT

public:
    explicit SoundManager(QObject* parent = nullptr);
    ~SoundManager();

public slots:
    void processBuffer(const QAudioBuffer&);

private slots:
    void togglePause();
    void toggleRecord();
    void updateStatus(QMediaRecorder::Status);
    void onStateChanged(QMediaRecorder::State);
    void updateProgress(qint64 pos);
    void displayErrorMessage();
    void playAudio();

private:
    Ui::MainWindow* m_gui = nullptr;
    MainWindow* m_mainWindow = nullptr;
    QAudioRecorder* m_qAudioRecorder = nullptr;
    QAudioProbe* m_probe = nullptr;
    QProcess* m_process = nullptr;

    QString m_ramdiskDir;
    QList<ProgressBar*> m_vumeterList;

    void clearAudioLevels();
};

#endif // SOUNDMANAGER_H
