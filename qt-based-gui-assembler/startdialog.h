#ifndef STARTDIALOG_H
#define STARTDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QString>
#include <QtGui>
#include <QTableWidget>
#include <QStringList>
#include <QDebug>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cstdio>
#include <algorithm>
#include <cstdlib>
#include <locale>
#include <QFileDialog>
#include <QDebug>
#include <fstream>
#include <cstdio>
#include <QMessageBox>
#include <QTextStream>

#include "atobfile_dialog.h"
#include "btoafile_dialog.h"
#include "simulate_mainwindow.h"

namespace Ui {
class StartDialog;
}

class StartDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StartDialog(QWidget *parent = 0);
    ~StartDialog();

private slots:

    void on_btn_aTobfile_clicked();

    void on_btn_bToafile_clicked();

    void on_btn_simulate_clicked();

private:
    Ui::StartDialog *ui;
};

#endif // STARTDIALOG_H
