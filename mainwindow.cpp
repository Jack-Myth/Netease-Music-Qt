#include "mainwindow.h"
#include "searchwindow.h"
#include "ui_mainwindow.h"
#include "globalsetting.h"
#include "utilitytools.h"
#include <QMenu>
#include <QMediaPlaylist>
#include <QMessageBox>

MainWindow* MainWindow::MyInstance=NULL;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    MusicPlayerCore=new QMediaPlayer();
    NetworkM=new QNetworkAccessManager();
    MyInstance=this;
    setAttribute(Qt::WA_DeleteOnClose);
    connect(MusicPlayerCore,&QMediaPlayer::stateChanged,this,&MainWindow::onMusicPlayerStateChanged,Qt::UniqueConnection);
    connect(MusicPlayerCore,&QMediaPlayer::positionChanged,this,&MainWindow::onMusicPlayerPositionChanged,Qt::UniqueConnection);
    connect(MusicPlayerCore,&QMediaPlayer::mediaStatusChanged,this,&MainWindow::mediaStatusChanged,Qt::UniqueConnection);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete MusicPlayerCore;
    delete NetworkM;
    MyInstance=NULL;
    exit(0);
}

void MainWindow::onMusicPlayerStateChanged(QMediaPlayer::State state)
{
    switch(state)
    {
    case QMediaPlayer::PlayingState:
        ui->PlayButton->setStyleSheet("border-image: url(:/Resources/pause.png);");
        break;
    case QMediaPlayer::PausedState:
    case QMediaPlayer::StoppedState:
        ui->PlayButton->setStyleSheet("border-image: url(:/Resources/play.png);");
        break;
    }
}



void MainWindow::onMusicPlayerPositionChanged(qint64 position)
{
    ui->ProgressSlider->setValue((float)position/MusicPlayerCore->duration()*ui->ProgressSlider->maximum());
    int Lrcline;
    if(LRCParserInstance.GetLyricLineIndex(position,Lrcline))
    {
        ui->LyricList->scrollToItem(ui->LyricList->item(Lrcline),QListWidget::PositionAtCenter);
        ui->LyricList->setCurrentRow(Lrcline);
    }
}

void MainWindow::mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    //return;
    if(status==QMediaPlayer::EndOfMedia)
    {
        if(!MusicCollection.size())
            return;
        CurrentMusic=(CurrentMusic+(CurrentMusic<0?MusicCollection.size():1))%MusicCollection.size();
        __CheckAndPlayMusic(MusicCollection[CurrentMusic]);
        __UpdatePlaylist();
    }
}

void MainWindow::PlayMusic()
{
    MusicPlayerCore->play();
}

void MainWindow::PauseMusic()
{
    MusicPlayerCore->pause();
}

void MainWindow::PlayMusic(MusicInfomation MusicToPlay,bool AddToPlaylist)
{
    disconnect(NetworkM,&QNetworkAccessManager::finished,0,0);
    connect(NetworkM,&QNetworkAccessManager::finished,this,[=](QNetworkReply* SearchResult)
    {
        auto SearchResultJson = QJsonDocument::fromJson(SearchResult->readAll()).object();
        auto url= SearchResultJson.find("data").value().toArray()[0].toObject().find("url").value().toString();
        QMediaContent mediaC(url);
        auto tmpMusicInfo=MusicToPlay;
        tmpMusicInfo.URL=url;
        if(AddToPlaylist)
        {
            if(CurrentMusic<0)
                if(MusicCollection.size()==0)
                    CurrentMusic=0;
                else
                    CurrentMusic+=MusicCollection.size();
            if(MusicCollection.count(tmpMusicInfo)<=0)
            {
                MusicCollection.insert(MusicCollection.begin()+CurrentMusic,tmpMusicInfo);
                __UpdatePlaylist();
            }
        }
        else
            CurrentMusic-=MusicCollection.size();
        __CheckAndPlayMusic(tmpMusicInfo);
    });
    int q=GlobalSetting::OnlineQuality <MusicToPlay.Quality.size()?GlobalSetting::OnlineQuality:MusicToPlay.Quality.size()-1;
    QString url=QString("https://api.imjad.cn/cloudmusic/?type=song&id=")
            +QString::asprintf("%d",MusicToPlay.ID)
            +QString("&br=")
            +QString::asprintf("%d",MusicToPlay.Quality[q]);
    NetworkM->get(QNetworkRequest(url));
}

void MainWindow::PlayMusicNext(MusicInfomation MusicToPlay)
{
    disconnect(NetworkM,&QNetworkAccessManager::finished,0,0);
    connect(NetworkM,&QNetworkAccessManager::finished,this,[=](QNetworkReply* SearchResult)
    {
        auto SearchResultJson = QJsonDocument::fromJson(SearchResult->readAll()).object();
        auto url= SearchResultJson.find("data").value().toArray()[0].toObject().find("url").value().toString();
        MusicInfomation tmpMusicinfo=MusicToPlay;
        tmpMusicinfo.URL=url;
        if(MusicCollection.count(tmpMusicinfo)>0)
            MusicCollection.removeAll(tmpMusicinfo);
        if(CurrentMusic<0)
        {
            if(!MusicCollection.size())
                MusicCollection.insert(MusicCollection.begin(),tmpMusicinfo);
            else
            {
                MusicCollection.insert(MusicCollection.begin()+CurrentMusic+MusicCollection.size(),tmpMusicinfo);
                CurrentMusic--;
            }
        }
        else
            MusicCollection.insert(MusicCollection.begin()+CurrentMusic+1,tmpMusicinfo);
        __UpdatePlaylist();
    });
    int q=GlobalSetting::OnlineQuality <MusicToPlay.Quality.size()?GlobalSetting::OnlineQuality:MusicToPlay.Quality.size()-1;
    QString url=QString("https://api.imjad.cn/cloudmusic/?type=song&id=")
            +QString::asprintf("%d",MusicToPlay.ID)
            +QString("&br=")
            +QString::asprintf("%d",MusicToPlay.Quality[q]);
    NetworkM->get(QNetworkRequest(url));
}

void MainWindow::PlayNext()
{
    CurrentMusic=(CurrentMusic+(CurrentMusic<0?MusicCollection.size():1))%MusicCollection.size();
    __CheckAndPlayMusic(MusicCollection[CurrentMusic]);
    __UpdatePlaylist();
}

void MainWindow::PlayPre()
{
    CurrentMusic=(CurrentMusic+(CurrentMusic<0?MusicCollection.size():MusicCollection.size()-1))%MusicCollection.size();
    __CheckAndPlayMusic(MusicCollection[CurrentMusic]);
    __UpdatePlaylist();
}

void MainWindow::__CheckAndPlayMusic(MusicInfomation MusicToPlay)
{
    ui->Music_Label->setText(MusicToPlay.Name);
    ui->AlbumName_Label->setText(u8"专辑:" + MusicToPlay.Album.Name);
    ui->ArtistName_Label->setText(u8"艺术家:" +MusicToPlay.ArtistsName);
    ui->Small_Music_Artist->setText(MusicToPlay.Name+" - "+MusicToPlay.ArtistsName);
    new GetPictureFromURL(MusicToPlay.Album.PicURL,this,[this](QPixmap pixmap){ui->MusicAvater->setPixmap(pixmap);});
    CurrentMusicInfo=MusicToPlay;
    MusicPlayerCore->setMedia(QMediaContent(MusicToPlay.URL));
    MusicPlayerCore->play();
    LRCParserInstance.ClearLyric();
    ui->LyricList->clear();
    disconnect(NetworkM,&QNetworkAccessManager::finished,0,0);
    connect(NetworkM,&QNetworkAccessManager::finished,this,[=](QNetworkReply* LyricResult) //尝试获取歌词
    {
        QJsonObject LrcJson=QJsonDocument::fromJson(LyricResult->readAll()).object();
        if(LrcJson.find("uncollected")==LrcJson.end()||LrcJson.find("uncollected").value().toBool()==false)
        {
            auto lrc=LrcJson.find("lrc").value().toObject().find("lyric").value().toString();
            LRCParserInstance.ParseLRCString(lrc);
            auto lrcD= LRCParserInstance.GetLyricData();
            for(int i=0;i<lrcD.size();i++)
            {
                ui->LyricList->addItem(lrcD[i].Line);
            }
        }
    });
    NetworkM->get(QNetworkRequest("https://api.imjad.cn/cloudmusic/?type=lyric&id="+QString::asprintf("%d",MusicToPlay.ID)));
}

void MainWindow::__UpdatePlaylist()
{
    ui->PlayList->clear();
    if(MusicCollection.size()<=0)
        return;
    int i=CurrentMusic<0?CurrentMusic+MusicCollection.size():CurrentMusic;
    int max=i;
    do
    {
        ui->PlayList->addItem(MusicCollection[i].Name);
        i=(i+1)%MusicCollection.size();
    }while(i!=max);
}

MusicInfomation MainWindow::GetCurrentMusic()
{
    return MusicCollection[CurrentMusic];
}

QVector<MusicInfomation> MainWindow::GetMusicInfolist()
{
    return MusicCollection;
}

void MainWindow::on_SearchWindowButton_clicked()
{
    if(SearchWindow::MyInstance)
        SearchWindow::MyInstance->raise();
    else
        (new SearchWindow())->show();
}

void MainWindow::on_ProgressSlider_valueChanged(int value)
{
    ui->TimeLabel->setText(QString::asprintf("%02d:%02d / %02d:%02d",
                                             MusicPlayerCore->position()/60000,MusicPlayerCore->position()%60000/1000,
                                             MusicPlayerCore->duration()/60000,MusicPlayerCore->duration()%60000/1000));
    if(!SliderPressing)
        ui->ProgressSlider->setSliderPosition(value);
}

void MainWindow::on_ProgressSlider_sliderPressed()
{
    SliderPressing=true;
}

void MainWindow::on_ProgressSlider_sliderReleased()
{
    MusicPlayerCore->setPosition((float)ui->ProgressSlider->sliderPosition()/ui->ProgressSlider->maximum()*MusicPlayerCore->duration());
    SliderPressing=false;
}

void MainWindow::on_PlayButton_clicked()
{
    //QMessageBox::information(NULL,"a",MusicPlayerCore->isSeekable()?"true":"false",QMessageBox::Ok);
    if(MusicPlayerCore->state()==QMediaPlayer::PlayingState)
        MusicPlayerCore->pause();
    else
        MusicPlayerCore->play();
}

void MainWindow::on_PreButton_clicked()
{
    PlayPre();
}

void MainWindow::on_NextButton_clicked()
{
    PlayNext();
}

void MainWindow::on_Share_Button_clicked()
{

    auto menu= UtitlityTools::ConstructShareMenu(CurrentMusicInfo);
    menu->exec(QCursor::pos());
    delete menu;
}
