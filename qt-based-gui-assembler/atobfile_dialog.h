#ifndef ATOBFILE_DIALOG_H
#define ATOBFILE_DIALOG_H

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

//#include "basic.h"
using namespace std;

namespace Ui {
class aTobfile_Dialog;
}

class aTobfile_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit aTobfile_Dialog(QWidget *parent = 0);
    ~aTobfile_Dialog();
    bool scan_label(string &source);
    bool stringAnalyze(string &source,unsigned int & bin);
    int register_map(string & s);
    void string_split(string & source,vector<std::string> & result);

private slots:
    void on_btn_open_clicked();

    bool on_btn_save_clicked();

    void on_btn_run_clicked();

    void on_btn_generateBin_clicked();

    void on_btn_generateCoe_clicked();

private:
    Ui::aTobfile_Dialog *ui;
    int PC;
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
    struct node
    {
        string labelName;
        unsigned int pos;
    };
    typedef struct node Label;
    Label label_list[1024];
    int lp;
    unsigned int binArray[1024];
    int bp;
    string filename;
    QString coutString;
};

#endif // ATOBFILE_DIALOG_H
