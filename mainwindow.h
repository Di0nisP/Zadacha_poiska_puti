#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <mygraphicview.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_Generation_clicked();

private:
    Ui::MainWindow *ui;
    MyGraphicView  *myPicture;  // Наш кастомный виджет
};

#endif // MAINWINDOW_H
