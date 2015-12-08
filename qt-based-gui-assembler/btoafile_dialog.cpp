#include "btoafile_dialog.h"
#include "ui_btoafile_dialog.h"

bToafile_Dialog::bToafile_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::bToafile_Dialog)
{
    ui->setupUi(this);
}

bToafile_Dialog::~bToafile_Dialog()
{
    delete ui;
}

void bToafile_Dialog::on_btn_open_clicked()
{
    coutString="";
    this->ui->asmTextEdit->clear();
    this->ui->consoleTextEdit->clear();

    QString fn=QFileDialog::getOpenFileName(
                    this,
                    tr("Open Files"),
                    QDir::homePath()
                    );
    filename=fn.toStdString();
    if(fn=="")
        coutString="Empty filename!!\n";
    else
        coutString="Open file: "+fn+" ok!\n";
    this->ui->consoleTextEdit->setText(coutString);
    coutString="";
}

void bToafile_Dialog::on_btn_convert_clicked()
{
    FILE * machineFile;

    machineFile=fopen(filename.c_str(),"rb");
    if(machineFile==NULL)
    {
        QMessageBox::critical(this, tr("Error"), tr("Could not open file"));
        return;
    }
    PC=0;
    instrSet="";
    while(!feof(machineFile))
    {
        unsigned int bin=0;
        bool complete=true;
        unsigned char sbin[4];

        for(int i=0;i<4;i++)
        {
            if(fread(&sbin[i],sizeof(unsigned char),1,machineFile)!=1)
            {
                coutString+="read from machineFile failed!\n";
                complete=false;
                break;
            }
            // printf("%08x\n", sbin[i]);

        }
        if(complete)
        {
            bin+=sbin[0]<<24;
            // printf("1:%08x\n", bin);
            bin+=sbin[1]<<16;
            // printf("2:%08x\n", bin);
            bin+=sbin[2]<<8;
            // printf("3:%08x\n", bin);
            bin+=sbin[3];
            // printf("4:%08x\n", bin);

            string instruction;
            binaryAnalyze(instruction,bin);
            instrSet+=QString::fromStdString(instruction)+"\n";
        }
    }
    fclose(machineFile);
    this->ui->consoleTextEdit->setText(coutString);
    this->ui->asmTextEdit->setText(instrSet);
}

void bToafile_Dialog::on_btn_save_clicked()
{
    filename+=".txt";
    QFile outfile(QString::fromStdString(filename));
    if(outfile.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        QTextStream stream(&outfile);
        stream<<instrSet;
    }
    coutString="Save data in "+QString::fromStdString(filename)+" OK!";
    this->ui->consoleTextEdit->setText(coutString);
}

bool bToafile_Dialog::binaryAnalyze(string & instruction,unsigned int &bin)
{
    unsigned int opcode=0,rs=0,rt=0,rd=0,shamt=0,funct=0;
    int immediate;
    unsigned int uimmediate;
    unsigned int address;
    unsigned int temp=bin;
    string OP,V1,V2,V3;

    instruction+=QString::number(PC).toStdString()+": ";
    opcode=bin>>26;

    rs=bin<<6;
    rs=rs>>27;

    rt=bin<<11;
    rt=rt>>27;

    rd=bin<<16;
    rd=rd>>27;

    shamt=bin<<21;
    shamt=shamt>>27;

    funct=bin<<26;
    funct=funct>>26;

    immediate=bin<<16;
    immediate=immediate>>16;

    uimmediate=bin<<16;
    uimmediate=uimmediate>>16;

    address=bin<<6;
    address=address>>6;


// R type
    if(opcode==0)
    {
        V1=bin_to_reg[rd];

        if(funct==0 || funct==2 || funct==3)   // shift
        {
            V2=bin_to_reg[rt];
            V3=QString::number(shamt).toStdString();
            switch(funct)
            {
                case 0:OP="sll";break;
                case 2:OP="srl";break;
                case 3:OP="sra";break;
                default:coutString+="invalid shift funct code!\n";return false;
            }
        }
        else
        {
            V2=bin_to_reg[rs];
            V3=bin_to_reg[rt];
            switch(funct)
            {
                case 32:OP="add";break;
                case 33:OP="addu";break;
                case 34:OP="sub";break;
                case 35:OP="subu";break;
                case 36:OP="and";break;
                case 37:OP="or";break;
                case 38:OP="xor";break;
                case 39:OP="nor";break;
                case 42:OP="slt";break;
                case 4:OP="sllv";break;
                case 6:OP="srlv";break;
                case 7:OP="srav";break;
                case 8:OP="jr";break;
            default:coutString+="At Postion:"+QString::number(PC)+"\ninvalid R type funct code !\n";return false;
            }
        }

        if(funct==8)     // one variable
        {
            OP="jr";
            V1=bin_to_reg[rs];
            instruction+=OP+" "+V1;
        }
        else          // three variables
            instruction+=OP+" "+V1+","+V2+","+V3;

        coutString+="----------------------------------------------\n";
        coutString+="binary="+QString("%1").arg(bin,8,16, QChar('0')).toUpper()+"\n";
        coutString+="opcode\trs\trt\trd\tshamt\tfunct\n";
        coutString+=""+QString::number(opcode)+"\t"+QString::number(rs)+"\t"+QString::number(rt)+"\t"+QString::number(rd)+"\t"+QString::number(shamt)+"\t"+QString::number(funct)+"\n";
        coutString+=QString::fromStdString(instruction)+"\n";
        coutString+="----------------------------------------------\n";
    }
// J type
    else if(opcode==2 || opcode==3)
    {
        if(opcode==2)
            OP="j";
        else
            OP="jal";

        V1=QString::number(address*4).toStdString();

        instruction+=OP+" "+V1;

        coutString+="----------------------------------------------\n";
        coutString+="binary="+QString("%1").arg(bin,8,16, QChar('0')).toUpper()+"\n";
        coutString+="opcode\taddress\n";
        coutString+=""+QString::number(opcode)+"\t"+QString::number(address)+"\n";
        coutString+=QString::fromStdString(instruction)+"\n";
        coutString+="----------------------------------------------\n";
    }
///   I type
    else
    {
        if(opcode==15)   // lui: two varialbes
        {
            OP="lui";
            V1=bin_to_reg[rt];
            V2=QString::number(immediate).toStdString();

            instruction+=OP+" "+V1+","+V2;
        }
        else if(opcode==35 || opcode==43)  // lw and sw: three varialbes but strange format
        {
            if(opcode==35)
                OP="lw";
            else
                OP="sw";

            V1=bin_to_reg[rt];
            V2=QString::number(immediate).toStdString();
            V3=bin_to_reg[rs];

            instruction+=OP+" "+V1+","+V2+"("+V3+")";
        }
        else   // three variables
        {
            V1=bin_to_reg[rt];
            V2=bin_to_reg[rs];
            if(opcode==4||opcode==5)
                V3=QString::number(PC+4+immediate*4).toStdString();
            else if(opcode==12||opcode==13||opcode==14)   // bit operation use unsigned immediate
                V3=QString::number(uimmediate).toStdString();
            else
                V3=QString::number(immediate).toStdString();
            printf("%08x\n",immediate );
            switch(opcode)
            {
                case 8:OP="addi";break;
                case 9:OP="addiu";break;
                case 12:OP="andi";break;
                case 13:OP="ori";break;
                case 14:OP="xori";break;
                case 4:OP="beq";break;
                case 5:OP="bne";break;
                case 10:OP="slti";break;
                case 11:OP="sltiu";break;
                default:coutString+"invalid I type opcode!\n";return false;
            }

            instruction+=OP+" "+V1+","+V2+","+V3;
        }

        coutString+="----------------------------------------------\n";
        coutString+="binary="+QString("%1").arg(bin,8,16, QChar('0')).toUpper()+"\n";
        coutString+="opcode\trs\trt\timmediate\n";
        coutString+=""+QString::number(opcode)+"\t"+QString::number(rs)+"\t"+QString::number(rt)+"\t"+QString::number(immediate)+"\n";
        coutString+=QString::fromStdString(instruction)+"\n";
        coutString+="----------------------------------------------\n";
    }
    PC+=4;

    return true;
}
