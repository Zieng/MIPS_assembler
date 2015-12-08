#include "simulate_mainwindow.h"
#include "ui_simulate_mainwindow.h"

simulate_MainWindow::simulate_MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::simulate_MainWindow)
{
    ui->setupUi(this);
}

simulate_MainWindow::~simulate_MainWindow()
{
    delete ui;
}

void simulate_MainWindow::on_btn_open_clicked()
{
    lp=bp=0;
    ui->memStateLabel->setText("Mem:unitial!");
    QString fn=QFileDialog::getOpenFileName(
                    this,
                    tr("Open Files"),
                    QDir::homePath()
                    );
    QFile file(fn);
    coutString="";
    ui->consoleTextEdit->clear();
    ui->asmTextEdit->clear();
    ui->binTextEdit->clear();
    filename=fn.toStdString();
    if(fn.endsWith(".bin",Qt::CaseSensitive)) //binary file
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
        binSet="";
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
                bin+=sbin[1]<<16;
                bin+=sbin[2]<<8;
                bin+=sbin[3];
                binSet+=QString("%1").arg(bin,8,16, QChar('0')).toUpper()+"\n";

                string instruction;
                binaryAnalyze(instruction,bin);
                instrSet+=QString::fromStdString(instruction)+"\n";
            }
        }
        fclose(machineFile);
        ui->asmTextEdit->setText(instrSet);
        ui->binTextEdit->setText(binSet);
    }
    else
    {
        if (!file.open(QFile::ReadOnly |QFile::Text))
        {
            QMessageBox::critical(this, tr("Error"), tr("Could not open file"));
            return;
        }
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
        bp=lp=PC=0;
        coutString="";
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
    }
    instrList=ui->asmTextEdit->toPlainText().split("\n");
    binList=ui->binTextEdit->toPlainText().split("\n");
    ui->consoleTextEdit->setText(coutString);
    on_btn_reset_clicked();
}

void simulate_MainWindow::on_btn_loadmem_clicked()
{
    QString fn=QFileDialog::getOpenFileName(
                    this,
                    tr("Open Files"),
                    QDir::homePath(),
                    tr("COE (*.coe);;All Files(*.*)" )
                    );
    QFile file(fn);
    QStringList dataList;
    coutString="";
    if(file.open(QFile::ReadOnly |QFile::Text))
    {
        QTextStream s(&file);
        QString t=s.readAll().replace(" ","");
        QStringList qsl=t.split(";");
        t=qsl[1].replace("\n","");
        qsl=t.split("=");
        t=qsl[1];
        dataList=t.split(",");
    }
    mp=dataList.length();
    for(int i=0;i<dataList.length();i++)
    {
        //coutString+=QString::number(dataList[i].toInt(0,16))+"\n";
        mem[i]=dataList[i].toUInt(0,16);
    }
    ui->memStateLabel->setText("Mem ok!");
    //ui->consoleTextEdit->setText(coutString);
}

void simulate_MainWindow::on_btn_memquery_clicked()
{
    int memAddr=ui->memAddrLineEdit->text().toInt();
    if(memAddr<0)
    {
        QMessageBox::critical(this, tr("Error"), tr("memory address must be posive!!"));
        return;
    }
    else if(memAddr>1024)
    {
        QMessageBox::critical(this, tr("Error"), tr("memory address is too large!!"));
        return;
    }
    QString temp=QString("%1").arg(mem[memAddr],8,16, QChar('0')).toUpper();
    /*For bases other than 10, mem[memAddr] is treated as an unsigned integer.*/
    ui->memDataLineEdit->setText(temp);
}

void simulate_MainWindow::on_btn_showMem_clicked()
{
    coutString="";
    ui->consoleTextEdit->clear();
    for(int i=0;i<mp;i++)
    {
        coutString+="At Postion:"+QString::number(i)+" "+QString("%1").arg(mem[i],8,16, QChar('0')).toUpper()+"\n";
    }
    ui->consoleTextEdit->setText(coutString);
}

void simulate_MainWindow::on_btn_next_clicked()
{
    PC=nextPC;
    coutString="";
    display_all();
    if((PC/4)>=binList.length())
    {
        coutString="\n\n-----------------------\n\n";
        coutString+="The program is end.";
        coutString+="\n\n-----------------------\n\n";
        ui->consoleTextEdit->setText(coutString);
        return;
    }
    simulate();
    if(nextPC==PC)
        nextPC=PC+4;
    display_all();
}

void simulate_MainWindow::on_btn_reset_clicked()
{
    PC=0;
    ui->startLineEdit->setText("0");
    for(int i=0;i<32;i++)
        r[i]=0;
    coutString="";
    display_all();
}

void simulate_MainWindow::on_btn_start_clicked()
{
    coutString="";
    PC=ui->startLineEdit->text().toInt();
    if((PC/4)>=binList.length())
    {
        QMessageBox::critical(this, tr("Error"), tr("PC value is beyond than what the assembler program used!!"));
        return;
    }
    if(PC<0)
    {
        QMessageBox::critical(this, tr("Error"), tr("PC must be positve!!"));
        return;
    }
    nextPC=PC+4;
    simulate();
}

void simulate_MainWindow::display_all()
{
    ui->pcLineEdit->setText(QString("%1").arg(PC,8,16, QChar('0')).toUpper());
    ui->nextPCLineEdit->setText(QString("%1").arg(nextPC,8,16, QChar('0')).toUpper());
    ui->asmLineEdit->setText(currInstr);
    ui->binLineEdit->setText(currBin);
    coutString+=QString::fromStdString(v0)+" "+QString::fromStdString(v1)+" "+QString::fromStdString(v2)\
            +" "+QString::fromStdString(v3)+"\n";
    coutString+="RS="+QString::number(RS)+" RT="+QString::number(RT)+" RD="+QString::number(RD)+"\n";
    coutString+="immediate="+QString::number(immediate)+" jaddress="+QString::number(address)+"\n";
    coutString+="*********************************\n";
    ui->consoleTextEdit->setText(coutString);
    QStringList regs;
    for(int i=0;i<32;i++)
    {
        regs<<QString("%1").arg(r[i],8,16, QChar('0')).toUpper();
    }
    ui->r0->setText(regs[0]);
    ui->r1->setText(regs[1]);
    ui->r2->setText(regs[2]);
    ui->r3->setText(regs[3]);
    ui->r4->setText(regs[4]);
    ui->r5->setText(regs[5]);
    ui->r6->setText(regs[6]);
    ui->r7->setText(regs[7]);
    ui->r8->setText(regs[8]);
    ui->r9->setText(regs[9]);
    ui->r10->setText(regs[10]);
    ui->r11->setText(regs[11]);
    ui->r12->setText(regs[12]);
    ui->r13->setText(regs[13]);
    ui->r14->setText(regs[14]);
    ui->r15->setText(regs[15]);
    ui->r16->setText(regs[16]);
    ui->r17->setText(regs[17]);
    ui->r18->setText(regs[18]);
    ui->r19->setText(regs[19]);
    ui->r20->setText(regs[20]);
    ui->r21->setText(regs[21]);
    ui->r22->setText(regs[22]);
    ui->r23->setText(regs[23]);
    ui->r24->setText(regs[24]);
    ui->r25->setText(regs[25]);
    ui->r26->setText(regs[26]);
    ui->r27->setText(regs[27]);
    ui->r28->setText(regs[28]);
    ui->r29->setText(regs[29]);
    ui->r30->setText(regs[30]);
    ui->r31->setText(regs[31]);
}

void simulate_MainWindow::simulate()
{
    currInstr=instrList[PC/4];
    currBin=binList[PC/4];

    vector<string> result;
    string temp=currInstr.toStdString();
    string_split(temp,result);

    coutString="";
    RD=RS=RT=immediate=address=0;
    v0=v1=v2=v3="None";
    v0=result[0];
    v1=result[1];
    printf("v0=%s\nv1=%s\n",v0.c_str(),v1.c_str());
    if(v0=="jr"||v0=="j"||v0=="jal")  // 2 parametes
    {
        if(v0=="jr")
        {
            jr();
        }
        else if(v0=="j")
            j();
        else if(v0=="jal")
            jal();
    }
    else if(v0=="lui")    // 3 parameters
    {
        v2=result[2];
        lui();
    }
    else                 // 4 parameters
    {
        v2=result[2];
        v3=result[3];
        if(v0=="add")
            add();
        else if(v0=="addu")
            addu();
        else if(v0=="sub")
            sub();
        else if(v0=="subu")
            subu();
        else if(v0=="and")
            and_funct();
        else if(v0=="or")
            or_funct();
        else if(v0=="xor")
            xor_funct();
        else if(v0=="nor")
            nor();
        else if(v0=="slt")
            slt();
        else if(v0=="sltu")
            sltu();
        else if(v0=="sll")
            sll();
        else if(v0=="srl")
            srl();
        else if(v0=="sllv")
            sllv();
        else if(v0=="srlv")
            srlv();
        else if(v0=="srav")
            srav();
        else if(v0=="addi")
            addi();
        else if(v0=="addiu")
            addiu();
        else if(v0=="andi")
            andi();
        else if(v0=="ori")
            ori();
        else if(v0=="xori")
            xori();
        else if(v0=="sw")
            sw();
        else if(v0=="lw")
            lw();
        else if(v0=="beq")
            beq();
        else if(v0=="bne")
            bne();
        else if(v0=="slti")
            slti();
        else if(v0=="sltiu")
            sltiu();
    }
    display_all();
}

bool simulate_MainWindow::scan_label(string &source)
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

bool simulate_MainWindow::stringAnalyze(string &source,unsigned int & bin)
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

int simulate_MainWindow::register_map(string & s)
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

void simulate_MainWindow::string_split(string & source,vector<std::string> & result)
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

bool simulate_MainWindow::binaryAnalyze(string & instruction,unsigned int &bin)
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

void simulate_MainWindow::add()
{
    RD=register_map(v1);
    RS=register_map(v2);
    RT=register_map(v3);
    r[RD]=r[RS]+r[RT];
}

void simulate_MainWindow::addu()
{
    RD=register_map(v1);
    RS=register_map(v2);
    RT=register_map(v3);
    r[RD]=(unsigned int)r[RS]+(unsigned int)r[RT];
}

void simulate_MainWindow::sub()
{
    RD=register_map(v1);
    RS=register_map(v2);
    RT=register_map(v3);
    r[RD]=r[RS]-r[RT];
}

void simulate_MainWindow::subu()
{
    RD=register_map(v1);
    RS=register_map(v2);
    RT=register_map(v3);
    r[RD]=(unsigned int)r[RS]-(unsigned int)r[RT];
}

void simulate_MainWindow::and_funct()
{
    RD=register_map(v1);
    RS=register_map(v2);
    RT=register_map(v3);
    r[RD]=r[RS]&r[RT];
}

void simulate_MainWindow::or_funct()
{
    RD=register_map(v1);
    RS=register_map(v2);
    RT=register_map(v3);
    r[RD]=r[RS]|r[RT];
}

void simulate_MainWindow::xor_funct()
{
    RD=register_map(v1);
    RS=register_map(v2);
    RT=register_map(v3);
    r[RD]=r[RS]^r[RT];
}

void simulate_MainWindow::nor()
{
    RD=register_map(v1);
    RS=register_map(v2);
    RT=register_map(v3);
    r[RD]=~(r[RS]|r[RT]);
}

void simulate_MainWindow::slt()
{
    RD=register_map(v1);
    RS=register_map(v2);
    RT=register_map(v3);
    r[RD]=(r[RS]<r[RT])?1:0;
}

void simulate_MainWindow::sltu()
{
    RD=register_map(v1);
    RS=register_map(v2);
    RT=register_map(v3);
    r[RD]=((unsigned int)r[RS]<(unsigned int)r[RT])?1:0;
}

void simulate_MainWindow::sll()
{
    RD=register_map(v1);
//    RS=register_map(v2);
    RT=register_map(v2);
    int shamt=QString::fromStdString(v3).toInt();
    r[RD]=(unsigned int)r[RT]<<shamt;
}

void simulate_MainWindow::srl()
{
    RD=register_map(v1);
//    RS=register_map(v2);
    RT=register_map(v2);
    int shamt=QString::fromStdString(v3).toInt();
    r[RD]=(unsigned int)r[RT]>>shamt;
}

void simulate_MainWindow::sra()
{
    RD=register_map(v1);
//    RS=register_map(v2);
    RT=register_map(v2);
    int shamt=QString::fromStdString(v3).toInt();
    r[RD]=(signed int)r[RT]>>shamt;
}

void simulate_MainWindow::sllv()
{
    RD=register_map(v1);
    RT=register_map(v2);
    RS=register_map(v3);
    r[RD]=(unsigned int)r[RT]<<r[RS];

}

void simulate_MainWindow::srlv()
{
    RD=register_map(v1);
    RT=register_map(v2);
    RS=register_map(v3);
    r[RD]=(unsigned int)r[RT]>>r[RS];
}

void simulate_MainWindow::srav()
{
    RD=register_map(v1);
    RT=register_map(v2);
    RS=register_map(v3);
    r[RD]=(signed int)r[RT]>>r[RS];
}

void simulate_MainWindow::jr()
{
    RS=register_map(v1);
    nextPC=r[RS];
}

//I-type functions
void simulate_MainWindow::addi()
{
    RT=register_map(v1);
    RS=register_map(v2);
    immediate=QString::fromStdString(v3).toInt();
    r[RT]=r[RS]+immediate;
}

void simulate_MainWindow::addiu()
{
    RT=register_map(v1);
    RS=register_map(v2);
    immediate=QString::fromStdString(v3).toInt();
    r[RT]=r[RS]+(unsigned int)immediate;
}

void simulate_MainWindow::andi()
{
    RT=register_map(v1);
    RS=register_map(v2);
    immediate=QString::fromStdString(v3).toInt();
    r[RT]=r[RS]&immediate;
}

void simulate_MainWindow::ori()
{
    RT=register_map(v1);
    RS=register_map(v2);
    immediate=QString::fromStdString(v3).toInt();
    r[RT]=r[RS]|immediate;
}

void simulate_MainWindow::xori()
{
    RT=register_map(v1);
    RS=register_map(v2);
    immediate=QString::fromStdString(v3).toInt();
    r[RT]=r[RS]^immediate;
}

void simulate_MainWindow::lui()
{
    RT=register_map(v1);
    immediate=QString::fromStdString(v2).toInt();
    r[RT]=immediate<<16;
}

void simulate_MainWindow::sw()
{
    RT=register_map(v1);
    RS=register_map(v2);
    immediate=QString::fromStdString(v3).toInt();
    mem[(r[RS]+immediate)/4]=r[RT];
}

void simulate_MainWindow::lw()
{
    RT=register_map(v1);
    RS=register_map(v2);
    immediate=QString::fromStdString(v3).toInt();
    r[RT]=mem[(r[RS]+immediate)/4];
}

void simulate_MainWindow::beq()
{
    RT=register_map(v1);
    RS=register_map(v2);
    int i=0;
    for(i=0;i<lp;i++)
    {
        if(v3==label_list[i].labelName)
        {
            address=label_list[i].pos;
            break;
        }
    }
    if(i==lp)
    {
        address=QString::fromStdString(v3).toInt();
    }
    if(r[RT]==r[RS])
        nextPC=address;
}

void simulate_MainWindow::bne()
{
    RT=register_map(v1);
    RS=register_map(v2);
    int i=0;
    for(i=0;i<lp;i++)
    {
        printf("Check the label:%s\n",label_list[i].labelName.c_str());
        if(v3==label_list[i].labelName)
        {
            address=label_list[i].pos;
            break;
        }
    }
    if(i==lp)
    {
        address=QString::fromStdString(v3).toInt();
    }
    if(r[RT]!=r[RS])
        nextPC=address;
}

void simulate_MainWindow::slti()
{
    RT=register_map(v1);
    RS=register_map(v2);
    immediate=QString::fromStdString(v3).toInt();
    r[RT]=(r[RS]<immediate)?1:0;
}

void simulate_MainWindow::sltiu()
{
    RT=register_map(v1);
    RS=register_map(v2);
    immediate=QString::fromStdString(v3).toInt();
    r[RT]=((unsigned int)r[RS]<(unsigned int)immediate)?1:0;
}

//J-type functions
void simulate_MainWindow::j()
{
    coutString="v0="+QString::fromStdString(v0)+" v1="+QString::fromStdString(v1)+"\n";
    int i=0;
    for(i=0;i<lp;i++)
    {
        if(v1==label_list[i].labelName)
        {
            address=label_list[i].pos;
            break;
        }
    }
    if(i==lp)
    {
        address=QString::fromStdString(v1).toInt();
    }
    nextPC=address;
}

void simulate_MainWindow::jal()
{
    int i=0;
    for(i=0;i<lp;i++)
    {
        if(v1==label_list[i].labelName)
        {
            address=label_list[i].pos;
            break;
        }
    }
    if(i==lp)
    {
        address=QString::fromStdString(v1).toInt();
    }
    r[31]=PC+4;
    nextPC=address;
}










