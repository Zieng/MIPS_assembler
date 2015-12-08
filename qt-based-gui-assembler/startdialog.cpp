#include "startdialog.h"
#include "ui_startdialog.h"

StartDialog::StartDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StartDialog)
{
    ui->setupUi(this);
}

StartDialog::~StartDialog()
{
    delete ui;
}


void StartDialog::on_btn_aTobfile_clicked()
{
    hide();
    aTobfile_Dialog *atbf=new aTobfile_Dialog();
    atbf->exec();
}


void StartDialog::on_btn_bToafile_clicked()
{
    hide();
    bToafile_Dialog *btaf=new bToafile_Dialog();
    btaf->exec();
}

void StartDialog::on_btn_simulate_clicked()
{
    hide();
    simulate_MainWindow *simu=new simulate_MainWindow();
    simu->show();
}
