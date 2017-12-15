﻿#include "downloadwindow.h"
#include "mainwindow.h"
#include "ui_downloadwindow.h"

DownloadWindow* DownloadWindow::MyInstance=NULL;
DownloadWindow::DownloadWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DownloadWindow)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    connect(&TimerForUpdate,&QTimer::timeout,this,&DownloadWindow::UpdateDownloadProgress,Qt::UniqueConnection);
    TimerForUpdate.start(1000);
}

DownloadWindow::~DownloadWindow()
{
    delete ui;
}

DownloadWindow *DownloadWindow::ShowAndRise()
{
    if(!MyInstance)
        MyInstance=new DownloadWindow;
    MyInstance->show();
    MyInstance->raise();
    return MyInstance;
}

void DownloadWindow::UpdateDownloadProgress()
{
    ui->DownloadList->clearContents();
    auto DownloadList= MainWindow::MyInstance->GetDownloadManager()->GetDownloadItems();
    ui->DownloadList->setRowCount(DownloadList.size());
    for(int i=0;i<DownloadList.size();i++)
    {
        ui->DownloadList->setItem(i,0,new QTableWidgetItem(DownloadList[i]->MusicInfo.Name));
        ui->DownloadList->setItem(i,1,new QTableWidgetItem(DownloadList[i]->MusicInfo.ArtistsName));
        ui->DownloadList->setItem(i,2,new QTableWidgetItem(DownloadList[i]->MusicInfo.Album.Name));
        ui->DownloadList->setItem(i,2,new QTableWidgetItem(QString("%1%").arg((int)((float)(DownloadList[i]->Downloaded)/(DownloadList[i]->Size)*1000)/10.f)));
    }
}
