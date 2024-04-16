#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QSettings>
#include <QMessageBox>
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
    MyGraphicView  *myPicture;  ///< Кастомный виджет

protected:
    /**
     * @brief Обработчк собиытия закрытия окна
     * @param event
     */
    void closeEvent(QCloseEvent *event) override;

    /**
     * @brief Метод чтения настроек положения окна
     */
    void readSettings();
};

#endif // MAINWINDOW_H
