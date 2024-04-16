#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setFixedSize(890, 620); // Фискируем размер окна

    myPicture = new MyGraphicView();

    ui->gridLayout->addWidget(myPicture);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_Generation_clicked()
{
    myPicture->numSquaresWidth  = ui->QLineEdit_Width->text().toInt();
    myPicture->numSquaresHeight = ui->QLineEdit_Height->text().toInt();
    myPicture->timer->start(50);
}
