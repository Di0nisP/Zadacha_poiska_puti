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

    ui->progressBar->setRange(0, 100); // Устанавливаем диапазон значений
    ui->progressBar->setValue(0);      // Устанавливаем текущее значение
    ui->progressBar->setVisible(false);

    readSettings();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete myPicture;
}

void MainWindow::on_Generation_clicked()
{
    ui->progressBar->setVisible(false);

    // Получение значения из QLineEdit:
    QString  widthText = ui->QLineEdit_Width ->text();
    QString heightText = ui->QLineEdit_Height->text();

    qreal numSquaresWidth  =  widthText.toInt();
    qreal numSquaresHeight = heightText.toInt();

    // Проверка, что введены только натруальные числа:
    bool isWidthNumber = !widthText.isEmpty() && numSquaresWidth > 0;
    bool isHeightNumber = !heightText.isEmpty() && numSquaresHeight > 0;

    if (!isWidthNumber || !isHeightNumber) {
        QMessageBox::warning(this, "Ошибка", "Введите натуральные числа (1, 2, 3 и т.д.)");
    } else if (numSquaresWidth * numSquaresHeight > 100 * 100 ){
        QMessageBox::warning(this, "Предупреждение", "Время ожидания увеличивается");
        myPicture->generate(numSquaresWidth, numSquaresHeight, ui->progressBar);
    } else {
        myPicture->generate(numSquaresWidth, numSquaresHeight, ui->progressBar);
    }
}

void inline MainWindow::readSettings()
{
    QSettings settings("./settings.ini", QSettings::IniFormat);
    QPoint position = settings.value("Position", QPoint(0, 0)).toPoint();
    move(position);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings("./settings.ini", QSettings::IniFormat);
    settings.setValue("Position", pos());
    QMainWindow::closeEvent(event);
}
