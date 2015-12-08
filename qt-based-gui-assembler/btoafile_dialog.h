#ifndef BTOAFILE_DIALOG_H
#define BTOAFILE_DIALOG_H

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
#include <QFile>

using namespace std;

namespace Ui {
class bToafile_Dialog;
}

class bToafile_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit bToafile_Dialog(QWidget *parent = 0);
    ~bToafile_Dialog();

    bool binaryAnalyze(string & instruction,unsigned int &bin);

private slots:
    void on_btn_open_clicked();

    void on_btn_convert_clicked();

    void on_btn_save_clicked();

private:
    Ui::bToafile_Dialog *ui;
    int PC;
    QString instrSet;
    string filename;
    QString coutString;
    char bin_to_reg[33][6]=
    {
        "$zero",
        "$at",
        "$v0",
        "$v1",
        "$a0",
        "$a1",
        "$a2",
        "$a3",
        "$t0",
        "$t1",
        "$t2",
        "$t3",
        "$t4",
        "$t5",
        "$t6",
        "$t7",
        "$s0",
        "$s1",
        "$s2",
        "$s3",
        "$s4",
        "$s5",
        "$s6",
        "$s7",
        "$t8",
        "$t9",
        "$k0",
        "$k1",
        "$gp",
        "$sp",
        "$fp",
        "$ra"
    };
};

#endif // BTOAFILE_DIALOG_H
