#ifndef SIMULATE_MAINWINDOW_H
#define SIMULATE_MAINWINDOW_H

#include <QMainWindow>
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
class simulate_MainWindow;
}

class simulate_MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit simulate_MainWindow(QWidget *parent = 0);
    ~simulate_MainWindow();
    bool binaryAnalyze(string & instruction,unsigned int &bin);
    bool scan_label(string &source);
    bool stringAnalyze(string &source,unsigned int & bin);
    int register_map(string & s);
    void string_split(string & source,vector<std::string> & result);
    void display_all();
    void simulate();
    //R-type functions
    void add();
    void addu();
    void sub();
    void subu();
    void and_funct();   //seems and/or/xor is keyword
    void or_funct();
    void xor_funct();
    void nor();
    void slt();
    void sltu();
    void sll();
    void srl();
    void sra();
    void sllv();
    void srlv();
    void srav();
    void jr();
    //I-type functions
    void addi();
    void addiu();
    void andi();
    void ori();
    void xori();
    void lui();
    void sw();
    void lw();
    void beq();
    void bne();
    void slti();
    void sltiu();
    //J-type functions
    void j();
    void jal();

private slots:
    void on_btn_open_clicked();

    void on_btn_next_clicked();

    void on_btn_reset_clicked();

    void on_btn_start_clicked();

    void on_btn_loadmem_clicked();

    void on_btn_memquery_clicked();

    void on_btn_showMem_clicked();

private:
    Ui::simulate_MainWindow *ui;
    int mem[1024];
    int r[32];
    string filename;
    QString coutString;
    unsigned int PC,nextPC;
    int lp;
    int mp;
    QString instrSet;
    QString binSet;
    struct node
    {
        string labelName;
        unsigned int pos;
    };
    typedef struct node Label;
    Label label_list[1024];
    unsigned int binArray[1024];
    int bp;
    QString currInstr;
    QString currBin;
    QStringList instrList;
    QStringList binList;
    string v0;
    string v1;
    string v2;
    string v3;
    int RS,RT,RD;
    int immediate,address;
    unsigned int uimmediate;
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

#endif // SIMULATE_MAINWINDOW_H
