#include "atobfile_dialog.h"
#include "ui_atobfile_dialog.h"

aTobfile_Dialog::aTobfile_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::aTobfile_Dialog)
{
    ui->setupUi(this);
    PC=0;
    bp=0;
    lp=0;
}

aTobfile_Dialog::~aTobfile_Dialog()
{
    delete ui;
}

void aTobfile_Dialog::on_btn_open_clicked()
{
    //clear all for a new start
    this->ui->asmTextEdit->clear();
    this->ui->binTextEdit->clear();
    this->ui->consoleTextEdit->clear();
    coutString="";
    PC=lp=bp=0;

    QString fn=QFileDialog::getOpenFileName(
                    this,
                    tr("Open Files"),
                    QDir::homePath()
                    );
    QFile file(fn);
    if (!file.open(QFile::ReadOnly |QFile::Text))
    {
        QMessageBox::critical(this, tr("Error"), tr("Could not open file"));
        return;
    }
    filename=fn.toStdString();
    QTextStream in(&file);
    while(!in.atEnd())
    {
        vector<std::string> result;
        QString line=in.readLine().toLower();
        QStringList qsl=line.split('#');  //remove the comments
        line=qsl[0];
        line.replace(QString("\t"),QString(" "));
        while(line.endsWith(" "))
            line.chop(1);
        string t=line.toStdString();
        string_split(t,result);
        if(line.contains("bgt"))
        {
            QString res_line1="slt $t0,"+QString::fromStdString(result[2])+","+QString::fromStdString(result[1]);
            QString res_line2="bne $t0,$zero,"+QString::fromStdString(result[3]);
            ui->asmTextEdit->append(res_line1);
            ui->asmTextEdit->append(res_line2);
        }
        else if(line.contains("blt"))
        {
            QString res_line1="slt $t0,"+QString::fromStdString(result[1])+","+QString::fromStdString(result[2]);
            QString res_line2="bne $t0,$zero,"+QString::fromStdString(result[3]);
            ui->asmTextEdit->append(res_line1);
            ui->asmTextEdit->append(res_line2);
        }
        else if(line.contains("bge"))
        {
            QString res_line1="slt $t0,"+QString::fromStdString(result[1])+","+QString::fromStdString(result[2]);
            QString res_line2="beq $t0,$zero,"+QString::fromStdString(result[3]);
            ui->asmTextEdit->append(res_line1);
            ui->asmTextEdit->append(res_line2);
        }
        else if(line.contains("ble"))
        {
            QString res_line1="slt $t0,"+QString::fromStdString(result[2])+","+QString::fromStdString(result[1]);
            QString res_line2="beq $t0,$zero,"+QString::fromStdString(result[3]);
            ui->asmTextEdit->append(res_line1);
            ui->asmTextEdit->append(res_line2);
        }
        else if(line.contains("move"))
        {
            QString res_line1="add "+QString::fromStdString(result[1])+","+QString::fromStdString(result[2]);
            ui->asmTextEdit->append(res_line1);
        }
        else if(!line.trimmed().isEmpty())
            this->ui->asmTextEdit->append(line);
    }
    //this->ui->asmTextEdit->setText(in.readAll().toLower());
    file.close();
}

bool aTobfile_Dialog::on_btn_save_clicked()
{
    QString fn = QFileDialog::getSaveFileName(this, tr("Save as..."),
                                                  QString(), tr("Text files (*.txt);;All Files (*)"));
    if (fn.isEmpty())
        return false;
    filename=fn.toStdString();
    QFile outfile;
    outfile.setFileName(fn);
    outfile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&outfile);
    out<<this->ui->asmTextEdit->toPlainText();
    outfile.flush();
    outfile.close();
    return true;
}

void aTobfile_Dialog::on_btn_run_clicked()
{
    bp=lp=PC=0;
    coutString="";
    this->ui->consoleTextEdit->clear();
    QString ss;
    ss=this->ui->asmTextEdit->toPlainText();
    QStringList sl=ss.split('\n');
    for(int i=0;i<sl.length();i++)   //scan for the label
    {
        QString line=sl[i];

        string lineBuffer;
        vector<string> seperate;

        lineBuffer=line.toStdString();
        if(scan_label(lineBuffer)==false)
        {
           coutString+="split line failed!\n";
           continue;
        }
    }
    PC=0;
    this->ui->binTextEdit->clear();
    for(int i=0;i<sl.length();i++)
    {
        QString line=sl[i];
        unsigned int bin=0;
        string lineBuffer=line.toStdString();
        if(stringAnalyze(lineBuffer,bin)==false)
        {
            coutString+="split line failed!\n";
            continue;
        }
        else
        {
            binArray[bp++]=bin;
            QString str=QString("%1").arg(bin,8,16, QChar('0')).toUpper();
            this->ui->binTextEdit->append(str);
        }
    }
    this->ui->consoleTextEdit->setText(coutString);
}

void aTobfile_Dialog::on_btn_generateBin_clicked()
{
    FILE * machineFile;

    on_btn_run_clicked();
    this->ui->consoleTextEdit->clear();
    coutString="";
    filename+=".bin";
    machineFile=fopen(filename.c_str(),"wb");
    if(machineFile==NULL)
    {
        coutString+="can not create the correspond machine code file!\n";
        return;
    }
    for(int j=0;j<bp;j++)
    {
        char sbin[4];
        unsigned int temp=binArray[j];

        for(int i=3;i>=0;i--)
        {
            sbin[i]=temp;
            temp=temp>>8;
        }
        for(int i=0;i<4;i++)
        {
            if(fwrite(&sbin[i],sizeof(char),1,machineFile)==1)
                coutString+="writting:"+QString("%1").arg(sbin[i],2,16, QChar('0')).toUpper()+"\n";
        }
    }

    if(fclose(machineFile)==0)
        coutString+="Write ok";
    this->ui->consoleTextEdit->setText(coutString);
}

void aTobfile_Dialog::on_btn_generateCoe_clicked()
{
    on_btn_run_clicked();
    filename+=".coe";
    QString fn=QString::fromStdString(filename);
    QFile coeFile(fn);
    this->ui->consoleTextEdit->clear();
    coutString="";

    if ( coeFile.open(QIODevice::WriteOnly|QIODevice::Text) )
    {
        QTextStream stream( &coeFile );
        coutString="Start writting...\n";
        stream<<"memory_initialization_radix=16;\n";
        stream<<"memory_initialization_vector=\n";

        for(int j=0;j<bp;j++)
        {
            QString temp=QString("%1").arg(binArray[j],8,16, QChar('0')).toLower();
            if(j!=bp-1)
                temp+=",\n";
            else
                temp+=";";
            stream<<temp;
            coutString+=temp+'\n';
        }
    }
    coutString+="write completed!\n";
    this->ui->consoleTextEdit->setText(coutString);
}

bool aTobfile_Dialog::scan_label(string &source)
{
    string subTemp;

    if(source.size()==0)
    {
        coutString+="Empty source!\n";
        return false;
    }

    for(string::iterator i=source.begin();i<=source.end();i++)
    {
        // cout<<"subTemp=:"<<subTemp<<endl;
        if(*i==':')   // indicate there is a label_destination
        {

            label_list[lp].labelName=subTemp;
            subTemp.clear();
            label_list[lp].pos=PC;
            coutString+="find a new label!\n";
            coutString+="label name:"+QString::fromStdString(label_list[lp].labelName)+" at "+QString::number(label_list[lp].pos)+"\n";
            lp++;
            break;
        }
        else
            subTemp+=*i;

    }

    PC+=4;
    return true;
}

bool aTobfile_Dialog::stringAnalyze(string &source,unsigned int & bin)
{
    string subTemp;
    vector<string>  result;
    unsigned int opcode,rs,rt,rd,shamt,funct;
    int immediate;
    unsigned int address;

    opcode=rs=rt=rd=shamt=funct=immediate=address=0;

    string_split(source,result);

    bin=0;
// R type
    string cmd=result[0];   // the first command
    if(cmd=="add" || cmd=="and" || cmd=="jr" || cmd=="or" || cmd=="slt" || cmd=="sll"
        || cmd=="srl" || cmd=="sub" || cmd=="xor" || cmd=="nor" || cmd=="addu"
        || cmd=="subu" || cmd=="sltu" || cmd=="sra" ||cmd=="sllv"||cmd=="srlv")
    {
        opcode=0;
        if(cmd=="jr")   // one operand
        {
            rs=register_map(result[1]);
            rd=rt=shamt=0;
            funct=8;   //0x0008
        }
        else if(cmd=="srl" || cmd=="sll" ||cmd=="sra") // two operands
        {
            rd=register_map(result[1]);
            rt=register_map(result[2]);
            rs=0;   //0x0000
            if(cmd=="sll")
                funct=0;
            else if(cmd=="srl")
                funct=2;   //0x0002
            else if(cmd=="sra")
                funct=3;
            // shamt=atoi(result[3].c_str());
            stringstream ss;
            ss<<result[3];
            ss>>shamt;
            printf("%08x\n",shamt );
        }
        else
        {
            rd=register_map(result[1]);
            rs=register_map(result[2]);
            rt=register_map(result[3]);
            shamt=0;
            if(cmd=="add")
                funct=32;
            else if(cmd=="addu")
                funct=33;
            else if(cmd=="and")
                funct=36;
            else if(cmd=="or")
                funct=37;
            else if(cmd=="slt")
                funct=42;
            else if(cmd=="sltu")
                funct=43;
            else if(cmd=="sub")
                funct=34;
            else if(cmd=="subu")
                funct=35;
            else if(cmd=="xor")
                funct=38;
            else if(cmd=="nor")
                funct=39;
            else if(cmd=="sllv")
                funct=4;
            else if(cmd=="srlv")
                funct=6;
            else if(cmd=="srav")
                funct=7;

        }
        bin+=funct;
        bin+=shamt<<6;
        bin+=rd<<11;
        bin+=rt<<16;
        bin+=rs<<21;
        bin+=opcode<<26;
        coutString+="********************************************************\n";
        coutString+="Postion:"+QString::number(PC)+" Check R type instruction:\n";
        for(unsigned int k=0;k<result.size();k++)
            coutString+=QString::fromStdString(result[k])+"\t";
        coutString+="\n";

        coutString+=QString::number(opcode);
        coutString+="\trs="+QString::number(rs);
        coutString+="\trt="+QString::number(rt);
        coutString+="\trd="+QString::number(rd);
        coutString+="\tshamt="+QString::number(shamt);
        coutString+="\tfunct="+QString::number(funct)+"\n";
        coutString+="instruction=";
        coutString+=QString("%1").arg(bin,8,16, QChar('0')).toUpper()+'\n';
        coutString+="********************************************************\n";
    }
/// I type
    else if(cmd=="andi" || cmd=="beq" || cmd=="bne" || cmd=="lw" || cmd=="ori"
        || cmd=="sw" || cmd=="addi"||cmd=="addiu"||cmd=="xori"||cmd=="addi"
        || cmd=="lui"||cmd=="slti" || cmd=="sltiu")
    {
        if(cmd=="lui")  // one operand
        {
            opcode=15;
            rt=register_map(result[1]);
            stringstream ss;
            ss<<result[2];
            ss>>immediate;
            rs=0;
        }
        else if(cmd=="lw" || cmd=="sw")
        {
            if(cmd=="lw")
                opcode=35;
            else
                opcode=43;
//            coutString+="the $rt before is"+QString::number(rt);
            rt=register_map(result[1]);
//            coutString+="the $rt after is"+QString::number(rt);
            stringstream ss;
            ss<<result[2];
            ss>>immediate;
            rs=register_map(result[3]);
        }
        else if(cmd=="andi" || cmd=="ori" || cmd=="addi"||cmd=="addiu"||cmd=="ori"||cmd=="xori"
            || cmd=="slti"||cmd=="sltiu")
        {
            if(cmd=="addi")
                opcode=8;
            else if(cmd=="addiu")
                opcode=9;
            else if(cmd=="andi")
                opcode=12;
            else if(cmd=="ori")
                opcode=13;
            else if(cmd=="xori")
                opcode=14;
            else if(cmd=="slti")
                opcode=10;
            else if(cmd=="sltiu")
                opcode=11;

            rt=register_map(result[1]);
            rs=register_map(result[2]);
            stringstream ss;
            ss<<result[3];
            ss>>immediate;
        }
        else if(cmd=="beq" || cmd=="bne")
        {
            if(cmd=="beq")
                opcode=4;
            else if(cmd=="bne")
                opcode=5;
            rs=register_map(result[1]);
            rt=register_map(result[2]);
            int k;
            for(k=0;k<lp;k++)
            {
                if(result[3]==label_list[k].labelName)
                {
                    coutString+="solve label "+QString::fromStdString(label_list[k].labelName)+" to address "+QString::number(label_list[k].pos)+"\n";
                    int y=label_list[k].pos,x=PC+4;
                    immediate=(y-x)/4;
                    break;
                }
            }
            if(k==lp)
            {
                coutString+="Error!Find a word is neither label or KEYWORDS\n";
            }
        }
        unsigned int x=immediate;
        x=x<<16;
        x=x>>16;
        bin+=x;
        bin+=rt<<16;
        bin+=rs<<21;
        bin+=opcode<<26;
        coutString+="********************************************************\n";
        coutString+="Position:"+QString::number(PC)+" Check I type instruction:\n";
        if(cmd!="lui")
            coutString+=QString::fromStdString(result[0])+"\t"+QString::fromStdString(result[1])+"\t"+QString::fromStdString(result[2])+"\t"+QString::fromStdString(result[3])+"\n";
        else
            coutString+=QString::fromStdString(result[0])+"\t"+QString::fromStdString(result[1])+"\t"+QString::fromStdString(result[2])+"\n";

        coutString+=QString::number(opcode);
        coutString+="\trs="+QString::number(rs);
        coutString+="\trt="+QString::number(rt);
        coutString+="\ti="+QString::number(immediate)+"\n";
        coutString+="instruction=";
        coutString+=QString("%1").arg(bin,8,16, QChar('0')).toUpper()+'\n';
        coutString+="********************************************************\n";

    }
/// J type
    else if(cmd=="j" || cmd=="jal")
    {
        if(cmd=="j")
            opcode=2;
        else if(cmd=="jal")
            opcode=3;
        int k;
        for(k=0;k<lp;k++)
        {
            if(result[1]==label_list[k].labelName)
            {
                address=label_list[k].pos/4;  // address is relative ?
                break;
            }
        }
        if(k==lp)  // indicate no correspond label ,may be just address
        {
            stringstream ss;
            ss<<result[1];
            ss>>address;
        }
        bin+=address;
        bin+=opcode<<26;
        coutString+="********************************************************\n";
        coutString+="Position:"+QString::number(PC)+" Check J type instruction:\n";
        coutString+=QString::fromStdString(result[0])+"\t"+QString::fromStdString(result[1])+"\n";

        coutString+=QString::number(opcode);
        coutString+="\taddress="+QString::number(address)+"\n";
        coutString+="instruction=";
        coutString+=QString("%1").arg(bin,8,16, QChar('0')).toUpper()+'\n';
        coutString+="********************************************************\n";
    }
    else
    {
        coutString+="undefined word!\n";
        return false;
    }

    PC+=4;
    return true;
}

int aTobfile_Dialog::register_map(string & s)
{
    unsigned int r=31;

    if(s=="$zero"||s=="$r0")
        r=0;
    else if(s=="$at"||s=="$r1")
        r=1;
    else if(s=="$v0"||s=="$r2")
        r=2;
    else if(s=="$v1"||s=="$r3")
        r=3;
    else if(s=="$a0"||s=="$r4")
        r=4;
    else if(s=="$a1"||s=="$r5")
        r=5;
    else if(s=="$a2"||s=="$r6")
        r=6;
    else if(s=="$a3"||s=="$r7")
        r=7;
    else if(s=="$t0"||s=="$r8")
        r=8;
    else if(s=="$t1"||s=="$r9")
        r=9;
    else if(s=="$t2"||s=="$r10")
        r=10;
    else if(s=="$t3"||s=="$r11")
        r=11;
    else if(s=="$t4"||s=="$r12")
        r=12;
    else if(s=="$t5"||s=="$r13")
        r=13;
    else if(s=="$t6"||s=="$r14")
        r=14;
    else if(s=="$t7"||s=="$r15")
        r=15;
    else if(s=="$s0"||s=="$r16")
        r=16;
    else if(s=="$s1"||s=="$r17")
        r=17;
    else if(s=="$s2"||s=="$r18")
        r=18;
    else if(s=="$s3"||s=="$r19")
        r=19;
    else if(s=="$s4"||s=="$r20")
        r=20;
    else if(s=="$s5"||s=="$r21")
        r=21;
    else if(s=="$s6"||s=="$r22")
        r=22;
    else if(s=="$s7"||s=="$r23")
        r=23;
    else if(s=="$t8"||s=="$r24")
        r=24;
    else if(s=="$t9"||s=="$r25")
        r=25;
    else if(s=="$k0"||s=="$r26")
        r=26;
    else if(s=="$k1"||s=="$r27")
        r=27;
    else if(s=="$gp"||s=="$r28")
        r=28;
    else if(s=="$sp"||s=="$r29")
        r=29;
    else if(s=="$fp"||s=="$r30")
        r=30;
    else if(s=="$ra"||s=="$r31")
        r=31;

    return r;
}

void aTobfile_Dialog::string_split(string & source,vector<std::string> & result)
{
    string subTemp;
    if(source.size()==0)
    {
        coutString+="Empty source!\n";
        return;
    }
    for(string::iterator i=source.begin();i<=source.end();i++)
    {
        // cout<<subTemp.size()<<" subTemp=:"<<subTemp<<endl;
        if(*i!=',' && *i!=' ' && *i!=')' && *i!='(' && *i!='\t' && i!=source.end()) //seperate
        {
            if(*i==':')     // skip the label
            {
                subTemp.clear();
                continue;
            }
            else
                subTemp+=*i;
        }
        else if(subTemp.size())
        {
            result.push_back(subTemp);
            // cout<<"length of subTemp befor push_back:"<<subTemp.size()<<"of "<<subTemp<<endl;
            subTemp.clear();
        }
    }
    // to remove all blacks
    for(unsigned int i=0;i<result.size();i++)
    {
        for(unsigned int j=0;j<result[i].length();j++)
            if(isspace(result[i][j]))
                result[i].erase(j,1);
//        result[i].erase(remove_if(result[i].begin(), result[i].end(), std::isspace), result[i].end());
    }
    for(int i=0;i<lp;i++)
    {
        for(unsigned int j=0;j<label_list[i].labelName.length();j++)
            if(isspace(label_list[i].labelName[j]))
                label_list[i].labelName.erase(j,1);
    }

    if(result.size()<2)
    {
        coutString+="too short !"+QString::number(result.size())+"\n";
        return ;
    }
    return;
}
