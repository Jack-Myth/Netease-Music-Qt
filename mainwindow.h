﻿#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mystruct.h"
#include "lrcparser.h"
#include <QMainWindow>
#include <QMediaPlayer>
#include <QtNetwork/QtNetwork>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    static MainWindow* MyInstance;
    void PlayMusic();
    void PlayMusic(struct MusicInfomation MusicToPlay,bool AddToPlayList=false);
    void PauseMusic();
    void mediaStatusChanged(QMediaPlayer::MediaStatus status);
    void PlayMusicNext(MusicInfomation MusicToPlay);
    MusicInfomation GetCurrentMusic();
    QVector<MusicInfomation> GetMusicInfolist();
    void PlayNext();
    void PlayPre();
private slots:

    void on_SearchWindowButton_clicked();

    void on_ProgressSlider_valueChanged(int value);

    void on_ProgressSlider_sliderPressed();

    void on_ProgressSlider_sliderReleased();

    void on_PlayButton_clicked();

    void on_PreButton_clicked();

    void on_NextButton_clicked();

    void on_Share_Button_clicked();

private:
    MusicInfomation CurrentMusicInfo={0};
    int CurrentMusic=-1;
    QVector<MusicInfomation> MusicCollection;
    Ui::MainWindow *ui;
    QMediaPlayer* MusicPlayerCore;
    LRCParser LRCParserInstance;
    class QNetworkAccessManager* NetworkM;
    bool SliderPressing=false;
    void onMusicPlayerStateChanged(QMediaPlayer::State state);
    void onMusicPlayerPositionChanged(qint64 position);
    void onMusicPlayerMediaChanged(const QMediaContent &media);
    void onPlaylistCurrentIndexChanged(int CurIndex);
    void __CheckAndPlayMusic(MusicInfomation MusicToPlay);
    void __UpdatePlaylist();
};

#endif // MAINWINDOW_H