#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "liblicense.h"

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
    void on_pbGenerate_clicked();

    void on_pbVerify_clicked();

private:
    Ui::MainWindow *ui;
    LibLicense serial;
};

#endif // MAINWINDOW_H
