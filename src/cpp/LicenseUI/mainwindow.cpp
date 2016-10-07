#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QString>
#include <QtSql>
#include <vector>
#include <functional>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("licensedb");
    db.setUserName("root");
    db.setPassword("");
    bool ok = db.open();

    QSqlQuery query(db);
    query.exec("select * from license");

    while (query.next())
    {
        QString license = query.value(1).toString();
        int id= query.value(0).toInt();
        qDebug() << id << ": " << license;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

std::string algo1(std::string seed)
{
    std::string res;
    res = "_" + seed + "_";
    return res;
}

void MainWindow::on_pbGenerate_clicked()
{
    std::string licenseholder = ui->txtLicensePrefix->text().trimmed().toStdString();

    std::vector<std::function<std::string(std::string)>> funcs;

    funcs.push_back(algo1); // prefix with underscores
    funcs.push_back([&](std::string seed) {return seed+"1.0.1";}); //version number
    funcs.push_back([&](std::string seed) {return seed+"1.0.2";});
    funcs.push_back([&](std::string seed) {return seed+"1.0.3";});
    funcs.push_back([&](std::string seed) {return seed+"1.0.4";});
    funcs.push_back([&](std::string seed) {return seed+"1.0.5";});
    funcs.push_back([&](std::string seed)
    {
        std::string res;
        res.resize(seed.size());

        std::reverse_copy(std::begin(seed), std::end(seed), std::begin(res));

        return res;
    });

    int groupsize = ui->txtGroupSize->text().toInt();
    std::string key = serial.generate(licenseholder, ui->cbLicensePartOfKey->isChecked(), groupsize, funcs);

    ui->txtLicenseKey->setPlainText(QString::fromStdString(key));
}

void MainWindow::on_pbVerify_clicked()
{
    std::string key = ui->txtLicenseKey->toPlainText().toStdString();

    // An application will do this: pick a few subkey algorithms and include with app binary
    // In this app, we pick two. For long subkeys, checking a single subkey is sufficient. In
    // fact, using several algorithms reveals information to potential keygen analyzers.
    auto res = serial.verify(key, algo1);
    if (res == LibLicense::Success)
        res = serial.verify(key, [&](std::string seed) {return seed+"1.0.5";});

    if (res == LibLicense::Verification::Success)
    {
        ui->lblMessage->setText("License key is valid");
    }
    else
    {
        QString msg = "License key is not valid: ";

        if (res == LibLicense::ChecksumFailure)
            msg += "Invalid checksum";
        else if (res == LibLicense::KeyMatchFailure)
            msg += "Key mismatch";
        else if (res == LibLicense::FormatFailure)
            msg += "Invalid format";
        else
            msg += "Unknown error";

         ui->lblMessage->setText(msg);
    }
}
