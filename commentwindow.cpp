#include "commentwindow.h"
#include "ui_commentwindow.h"

CommentWindow::CommentWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CommentWindow)
{
    ui->setupUi(this);
}

CommentWindow::~CommentWindow()
{
    delete ui;
}
