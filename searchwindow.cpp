#include "searchwindow.h"
#include "ui_searchwindow.h"
#include "mystruct.h"
#include "mainwindow.h"
#include "globalsetting.h"
#include "utilitytools.h"
#include <QPushButton>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QMessageBox>
#include <QMenu>
#include <QDesktopServices>
#include <QtNetwork/QtNetwork>

SearchWindow* SearchWindow::MyInstance=NULL;
SearchWindow::SearchWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SearchWindow)
{
    ui->setupUi(this);
    MyInstance=this;
    setAttribute(Qt::WA_DeleteOnClose);
    NetworkM=new QNetworkAccessManager();
    ui->progressBar->hide();
    ui->SearchResult_Music->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->SearchResult_Music->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->SearchResult_Music->setColumnWidth(0,300);//歌名
    ui->SearchResult_Music->setHorizontalHeaderLabels(QStringList({u8"歌曲",u8"艺术家",u8"专辑",u8"时长"}));
    ui->SearchResult_Music->setColumnWidth(1,195);//歌手名
    ui->SearchResult_Music->setColumnWidth(2,195);//专辑名
    ui->SearchResult_Music->setColumnWidth(3,50);//歌曲长度
    //ui->SearchResult_Music->verticalHeader()->hide();
    ui->SearchResult_Music->setWordWrap(false);
    ui->SearchResult_Music->setShowGrid(false);
    ui->SearchResult_Music->setSortingEnabled(false);
    ui->SearchResult_Music->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->SearchResult_Music->setSelectionMode(QAbstractItemView::SingleSelection);
    //ui->SearchResult_Music->horizontalHeader()->setHighlightSections(true);
}

SearchWindow::~SearchWindow()
{
    delete ui;
    delete NetworkM;
    MyInstance=NULL;
}

void SearchWindow::on_Search_lineEdit_editingFinished()
{
    if(ui->Search_lineEdit->hasFocus())
    {
        BeginSearch(0);
    }
}

void SearchWindow::on_pushButton_clicked()
{
    BeginSearch(0);
}

void SearchWindow::BeginSearch(int Page)
{
    ui->progressBar->show();
    switch(ui->SearchResult_tabWidget->currentIndex())
    {
    case 0:
    {
        SearchPage=Page;
        QString Aaa=QString::asprintf("https://api.imjad.cn/cloudmusic/?type=search&id=1&limit=%d&offset=%d&s=",GlobalSetting::SearchLimit,GlobalSetting::SearchLimit*Page)+QTextCodec::codecForName("utf-8")->fromUnicode(ui->Search_lineEdit->text()).toPercentEncoding();
        //disconnect(NetworkM,&QNetworkAccessManager::finished,nullptr,nullptr);
        QNetworkReply* SearchR= NetworkM->get(QNetworkRequest(QUrl(Aaa)));
        connect(SearchR,&QNetworkReply::finished,this,[=]()
        {
            SearchR->deleteLater();
            ui->progressBar->hide();
            if(SearchR->error())
            {
                QMessageBox::information(nullptr,"Error",SearchR->errorString(),QMessageBox::Ok);
                return;
            }
            //QMessageBox::information(nullptr,"","GetConnection",QMessageBox::Ok);
            auto JsonRawDataArray=SearchR->readAll();
            QJsonParseError JsonError;
            QJsonDocument JsonDocument= QJsonDocument::fromJson(JsonRawDataArray,&JsonError);
            if(JsonError.error != QJsonParseError::NoError)
            {
                QMessageBox::information(nullptr,"Error",JsonError.errorString(),QMessageBox::Ok);
                return;
            }
            LastSearchResult_Music.clear();
            ui->SearchResult_Music->clearContents();
            ui->SearchResult_Music->setRowCount(0);
            //第一层(result[],code)
            auto result= JsonDocument.object().find("result").value().toObject();
            //第二层(songs[],songCount)
            QJsonArray songs= result.find("songs").value().toArray();
            ui->SearchResult_Music->setRowCount(songs.size());
            //第三层(0[],1[]...n[])
            for(int i=0;i<songs.size();i++)
            {
                MusicInfomation Music;
                QJsonObject MusicItem= songs[i].toObject();
                //ui->SearchResult_Music->insertRow(1);
                Music.ID=MusicItem.find("id").value().toInt();
                Music.Name=MusicItem.find("name").value().toString();
                ui->SearchResult_Music->setItem(i,0,new QTableWidgetItem(Music.Name));//歌曲名
                QString Artists;
                auto ArtistList=MusicItem.find("ar").value().toArray();
                for(int a=0;a<ArtistList.size();a++)
                {
                    auto ArtistObject=ArtistList[a].toObject();
                    ArtistInfomation ArtistInfo;
                    ArtistInfo.ID=ArtistObject.find("id").value().toInt();
                    ArtistInfo.Name=ArtistObject.find("name").value().toString();
                    Music.Artists.push_back(ArtistInfo);
                    Artists+="/"+ArtistInfo.Name;
                }
                if(Artists.length()>0)
                    Artists=Artists.mid(1);
                Music.ArtistsName=Artists;
                ui->SearchResult_Music->setItem(i,1,new QTableWidgetItem(Artists));//艺术家
                auto AlbumInfo= MusicItem.find("al").value().toObject();
                Music.Album.ID=AlbumInfo.find("id").value().toInt();
                //Music.Album.PicID=AlbumInfo.find("pic").value().toString().toLongLong();
                Music.Album.Name=AlbumInfo.find("name").value().toString();
                Music.Album.PicURL=AlbumInfo.find("picUrl").value().toString();
                ui->SearchResult_Music->setItem(i,2,new QTableWidgetItem(Music.Album.Name));//专辑
                Music.Length=MusicItem.find("dt").value().toInt();
                ui->SearchResult_Music->setItem(i,3,new QTableWidgetItem(QString::asprintf("%d:%d",Music.Length/1000/60,Music.Length/1000%60))); //音乐长度
                auto it= MusicItem.find("l");
                if(it!=MusicItem.end())
                {
                    Music.Quality.push_back(it.value().toObject().find("br").value().toInt());
                }
                it= MusicItem.find("m");
                if(it!=MusicItem.end())
                {
                    Music.Quality.push_back(it.value().toObject().find("br").value().toInt());
                }
                it= MusicItem.find("h");
                if(it!=MusicItem.end())
                {
                    Music.Quality.push_back(it.value().toObject().find("br").value().toInt());
                }
                LastSearchResult_Music.push_back(Music);
            }
                ui->SearchPre_Button->setEnabled(Page>0?true:false);
                ui->SearchNext_Button->setEnabled(result.find("songCount").value().toInt()>Page*GlobalSetting::SearchLimit?true:false);
        });
    }
        break;
    case 1:case 2:case 3:case 4:
        ui->progressBar->hide();
    }
}

void SearchWindow::on_SearchResult_Music_cellDoubleClicked(int row, int column)
{
    (void)column;
    MainWindow::MyInstance->PlayMusic(LastSearchResult_Music[row]);
}


void SearchWindow::on_SearchPre_Button_clicked()
{
    BeginSearch(SearchPage-1);
}

void SearchWindow::on_SearchNext_Button_clicked()
{
    BeginSearch(SearchPage+1);
}

void SearchWindow::on_SearchResult_tabWidget_currentChanged(int index)
{
    switch(index)
    {
    case 0:
        ui->Search_lineEdit->setPlaceholderText(u8"搜索 音乐 专辑 艺术家");
        break;
    case 1:
        ui->Search_lineEdit->setPlaceholderText(u8"搜索 艺术家");
        break;
    case 2:
        ui->Search_lineEdit->setPlaceholderText(u8"搜索 专辑");
        break;
    case 3:
        ui->Search_lineEdit->setPlaceholderText(u8"搜索 歌词");
        break;
    case 4:
        ui->Search_lineEdit->setPlaceholderText(u8"搜索 电台");
        break;
    }
}

void SearchWindow::on_SearchResult_Music_customContextMenuRequested(const QPoint &pos)
{
    (void)pos;
    QMenu menu;
    int curRow=ui->SearchResult_Music->currentRow();
    switch(ui->SearchResult_Music->currentColumn())
    {
    case 0:
        menu.addAction(u8"立即播放",[=]()
        {
            MainWindow::MyInstance->PlayMusic(LastSearchResult_Music[curRow],true);
        });
        menu.addAction(u8"下一首播放",[=]()
        {
            MainWindow::MyInstance->PlayMusicNext(LastSearchResult_Music[curRow]);
        });
        break;
    case 1:
    case 2:
    case 3:
    case 4:
        break;
    }
    menu.addSeparator();
    menu.addAction(u8"下载",[=]()
    {
        QMessageBox::information(nullptr,"emmmmm","假装可以下载",QMessageBox::Ok);
    });
    menu.addMenu(UtitlityTools::ConstructShareMenu(LastSearchResult_Music[curRow]));
    if(QApplication::arguments().contains("-DevMode",Qt::CaseInsensitive))
        menu.addAction(QString::asprintf("%d",LastSearchResult_Music[curRow].ID));
    menu.exec(QCursor::pos());
}
