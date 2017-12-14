#include "commentwindow.h"
#include "mainwindow.h"
#include "ui_commentwindow.h"
#include "globalsetting.h"
#include "utilitytools.h"
#include <QLabel>
#include <QtNetwork>

CommentWindow* CommentWindow::MyInstance=NULL;
CommentWindow::CommentWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CommentWindow)
{
    if(MyInstance)
        throw 0;
    MyInstance=this;
    ui->setupUi(this);
    NetworkM=new QNetworkAccessManager();
    //setWindowFlag(Qt::FramelessWindowHint);
    SuperWndProc=(WNDPROC)SetWindowLongPtr((HWND)this->winId(),GWLP_WNDPROC,(LONG_PTR)&CommentWindow_MyWndProc);
    connect(this,&CommentWindow::WindowMoved,this,&CommentWindow::onWindowMoved,Qt::UniqueConnection);
    GetComment(MainWindow::MyInstance->GetCurrentMusic(),0);
}

LRESULT CALLBACK CommentWindow_MyWndProc(HWND wnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    if(message==WM_MOVE)
    {
        LRESULT returnv=CommentWindow::MyInstance->SuperWndProc(wnd,message,wParam,lParam);
        emit CommentWindow::MyInstance->WindowMoved();
        return returnv;
    }
    return CommentWindow::MyInstance->SuperWndProc(wnd,message,wParam,lParam);
}

CommentWindow::~CommentWindow()
{
    delete ui;
    MyInstance=NULL;
}

void CommentWindow::onWindowMoved()
{
    MainWindow::MyInstance->move(pos().x()-MainWindow::MyInstance->width()-2,pos().y());
}

void CommentWindow::AddComment(QJsonObject JsonObject)
{
    QWidget* widget=new QWidget();
    QLabel* Icon=new QLabel(widget);
    Icon->resize(50,50);
    Icon->move(0,0);
    QLabel* MainContent=new QLabel(widget);
    MainContent->setMaximumWidth(this->width()-51);
    MainContent->setMinimumWidth(this->width()-51);
    MainContent->setMinimumHeight(10);
    //MainContent->resize(MainContent->height(),MainContent->maximumWidth());
    MainContent->setAlignment(Qt::AlignTop);
    MainContent->move(50,0);
    MainContent->setWordWrap(true);
    QJsonObject user= JsonObject.find("user").value().toObject();
    QString AvatarURL=user.find("avatarUrl").value().toString();
    QString NickName=user.find("nickname").value().toString();
    QString Content=JsonObject.find("content").value().toString();
    MainContent->setText(NickName+QString(":")+ Content);
    MainContent->adjustSize();
    MainContent->resize(0,MainContent->height()+20);
    QJsonArray beReplied=JsonObject.find("beReplied").value().toArray();
    int lastY=MainContent->height()+5;
    for(int i=0;i<beReplied.size();i++)
    {
        QLabel* subContentLabel=new QLabel(widget);
        subContentLabel->setMaximumWidth(this->width()-61);
        subContentLabel->setMinimumWidth(this->width()-61);
        subContentLabel->setMinimumHeight(10);
        subContentLabel->setWordWrap(true);
        //subContentLabel->resize(subContentLabel->height(),subContentLabel->maximumWidth());
        subContentLabel->setAlignment(Qt::AlignTop);
        QJsonObject subuser= beReplied[i].toObject().find("user").value().toObject();
        subContentLabel->setStyleSheet("background-color: rgb(238, 238, 238);");
        QString subNickName=subuser.find("nickname").value().toString();
        QString subContent=beReplied[i].toObject().find("content").value().toString();
        subContentLabel->setText(subNickName+ QString(":")+ subContent);
        subContentLabel->adjustSize();
        subContentLabel->resize(0,subContentLabel->height()+20);
        subContentLabel->move(60,lastY);
        lastY+=subContentLabel->height()+5;
    }
    bool ExistSamePicUser=false;
    for(int k=0;k<SameLabelCollectionInstance.size();k++)
        if(SameLabelCollectionInstance[k]->url==AvatarURL)
        {
            SameLabelCollectionInstance[k]->LabelCollection.push_back(Icon);
            ExistSamePicUser=true;
            break;
        }
    if(!ExistSamePicUser)
    {
        SameLabelCollection* pSameLabelCollection=new SameLabelCollection();
        pSameLabelCollection->url=AvatarURL;
        pSameLabelCollection->LabelCollection.push_back(Icon);
        SameLabelCollectionInstance.push_back(pSameLabelCollection);
        pSameLabelCollection->pGetPictureFromURL=new GetPictureFromURL(AvatarURL,this,[=](QPixmap pixmap)
        {
            for(int i=0;i<pSameLabelCollection->LabelCollection.size();i++)
                pSameLabelCollection->LabelCollection[i]->setPixmap(pixmap.scaled(Icon->size(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
            delete pSameLabelCollection;
            SameLabelCollectionInstance.removeAll(pSameLabelCollection);
        });
    }
    auto ListItem=new QListWidgetItem(ui->CommentList);
    ui->CommentList->addItem(ListItem);
    ui->CommentList->setItemWidget(ListItem,widget);
    ListItem->setSizeHint(QSize(0,lastY>60?lastY:60));
}

void CommentWindow::GetComment(MusicInfomation Music,int Page)
{
    if(!Music.ID)
    {
        ui->progressBar->hide();
        ui->CommentList->clear();
        return;
    }
    ui->progressBar->show();
    CurrentPage=Page;
    disconnect(NetworkM,&QNetworkAccessManager::finished,0,0);
    connect(NetworkM,&QNetworkAccessManager::finished,this,[=](QNetworkReply* CommentResult)
    {
        ui->progressBar->hide();
        for(int i=0;i<SameLabelCollectionInstance.size();i++)
            SameLabelCollectionInstance[i]->Clear();
        SameLabelCollectionInstance.clear();
        ui->CommentList->clear();
        QJsonObject CommentRootJson=QJsonDocument::fromJson(CommentResult->readAll()).object();
        if(Page==0&&CommentRootJson.find("moreHot").value().toBool())
        {
            ui->CommentList->addItem(u8"热门评论:");
            QJsonArray HotComment=CommentRootJson.find("hotComments").value().toArray();
            for(int i=0;i<HotComment.size();i++)
            {
                AddComment(HotComment[i].toObject());
            }
        }
        QJsonArray Comments=CommentRootJson.find("comments").value().toArray();
        if(Comments.size()==0)
            ui->CommentList->addItem(u8"这首歌还没有人评论呢。");
        else
        {
            ui->CommentList->addItem(u8"最新评论:");
            for(int i=0;i<Comments.size();i++)
            {
                AddComment(Comments[i].toObject());
            }
        }
        if(CommentRootJson.find("more").value().toBool())
            ui->NextButton->setEnabled(true);
        else
            ui->NextButton->setEnabled(false);
        if(Page>0)
            ui->PreButton->setEnabled(true);
        else
            ui->PreButton->setEnabled(false);
        CommentResult->deleteLater();
    },Qt::UniqueConnection);
    NetworkM->get(QNetworkRequest(QString("https://api.imjad.cn/cloudmusic/?type=comments&id=%1&limit=%2&offset=%3").arg(Music.ID).arg(GlobalSetting::CommentLimit).arg(GlobalSetting::CommentLimit*Page)));
}

void CommentWindow::on_NextButton_clicked()
{
    GetComment(MainWindow::MyInstance->GetCurrentMusic(),CurrentPage+1);
}

void CommentWindow::on_PreButton_clicked()
{
    GetComment(MainWindow::MyInstance->GetCurrentMusic(),CurrentPage-1);
}

void CommentWindow::on_RefreshButton_clicked()
{
    GetComment(MainWindow::MyInstance->GetCurrentMusic(),CurrentPage);
}
