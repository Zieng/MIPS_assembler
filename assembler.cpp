/*
 * main.cpp
 *
 *  Created on: May 6, 2015
 *      Author: zieng
 */


#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdlib>

using namespace std;

enum reg_to_bin
{
	zero,
	at,
	v0,
	v1,
	a0,
	a1,
	a2,
	a3,
	t0,
	t1,
	t2,
	t3,
	t4,
	t5,
	t6,
	t7,
	s0,
	s1,
	s2,
	s3,
	s4,
	s5,
	s6,
	s7,
	t8,
	t9,
	k0,
	k1,
	gp,
	sp,
	fp,
	ra
};

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

char KEYWORDS[100][10]=
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
	"$ra",
	"add",
	"addu",
	"and",
	"jr",
	"nor",
	"or",
	"slt",
	"sltu",
	"sll",
	"srl",
	"sub",
	"subu",
	"addi",
	"addiu",
	"andi",
	"beq",
	"bne",
	"lbu",
	"lhu",
	"ll",
	"lui",
	"lw",
	"ori",
	"slti",
	"sltiu",
	"sb",
	"sc",
	"sh",
	"sw",
	"j",
	"jal"
};

struct node
{
	string labelName;
	unsigned int pos;
};
typedef struct node Label;
Label label_list[1024];
int lp=0;



unsigned int PC=0;

void single_asm_to_bin();
int single_bin_to_asm();
int asm_TO_bin();
int bin_TO_asm();
bool stringAnalyze(string &source,unsigned int &bin);
bool binaryAnalyze(string & instruction,unsigned int &bin);
bool scan_label(string &source);
bool register_map(string & s,unsigned int & r);



bool scan_label(string &source)
{
	string subTemp;

	
	if(source.size()==0)
	{
		cout<<"Empty source!"<<endl;
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
			cout<<"find a new label!"<<endl;
			cout<<"label name:"<<label_list[lp].labelName<<" at "<<label_list[lp].pos<<endl;
			lp++;
			break;
		}
		else
			subTemp+=*i;

	}


	PC+=4;
	return true;
}

bool register_map(string & s,unsigned int & r)
{
	if(s=="$zero")
		r=zero;
	else if(s=="$at")
		r=at;
	else if(s=="$v0")
		r=v0;
	else if(s=="$v1")
		r=v1;
	else if(s=="$a0")
		r=a0;
	else if(s=="$a1")
		r=a1;
	else if(s=="$a2")
		r=a2;
	else if(s=="$a3")
		r=a3;
	else if(s=="$t0")
		r=t0;
	else if(s=="$t1")
		r=t1;
	else if(s=="$t2")
		r=t2;
	else if(s=="$t3")
		r=t3;
	else if(s=="$t4")
		r=t4;
	else if(s=="$t5")
		r=t5;
	else if(s=="$t6")
		r=t6;
	else if(s=="$t7")
		r=t7;
	else if(s=="$s0")
		r=s0;
	else if(s=="$s1")
		r=s1;
	else if(s=="$s2")
		r=s2;
	else if(s=="$s3")
		r=s3;
	else if(s=="$s4")
		r=s4;
	else if(s=="$s5")
		r=s5;
	else if(s=="$s6")
		r=s6;
	else if(s=="$s7")
		r=s7;
	else if(s=="$t8")
		r=t8;
	else if(s=="$t9")
		r=t9;
	else if(s=="$k0")
		r=k0;
	else if(s=="$k1")
		r=k1;
	else if(s=="$gp")
		r=gp;
	else if(s=="$sp")
		r=sp;
	else if(s=="$fp")
		r=fp;
	else if(s=="$ra")
		r=ra;
	else
		return false;

	return true;
}

bool stringAnalyze(string &source,unsigned int & bin)
{
	string subTemp;
	vector<string>  result;
	unsigned int opcode,rs,rt,rd,shamt,funct;
	int immediate;
	unsigned int address;

	opcode=rs=rt=rd=shamt=funct=immediate=address=0;

	if(source.size()==0)
	{
		cout<<"Empty source!"<<endl;
		return false;
	}
	opcode=rs=rt=rd=shamt=funct=immediate=address=0;
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
	if(result.size()<2)
	{
		cout<<"too short !"<<result.size()<<endl;
		return false;
	}	
		

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
			register_map(result[1],rs);
			rd=rt=shamt=0;
			funct=8;   //0x0008
		}
		else if(cmd=="srl" || cmd=="sll" ||cmd=="sra") // two operands
		{
			register_map(result[1],rd);
			register_map(result[2],rt);
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
			register_map(result[1],rd);
			register_map(result[2],rs);
			register_map(result[3],rt);
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
		cout<<"********************************************************"<<endl;
		cout<<"Postion:"<<PC<<" Check R type instruction:"<<endl;
		for(int k=0;k<result.size();k++)
			cout<<result[k]<<"\t";
		cout<<endl;

		cout<<opcode;
		cout<<"\trs="<<rs;
		cout<<"\trt="<<rt;
		cout<<"\trd="<<rd;
		cout<<"\tshamt="<<shamt;
		cout<<"\tfunct="<<funct<<endl;
		cout<<"instruction=";
		printf("%08x\n",bin);
		cout<<"********************************************************"<<endl;
	}
/// I type
	else if(cmd=="andi" || cmd=="beq" || cmd=="bne" || cmd=="lw" || cmd=="ori"
		|| cmd=="sw" || cmd=="addi"||cmd=="addiu"||cmd=="xori"||cmd=="addi"
		|| cmd=="lui"||cmd=="slti" || cmd=="sltiu")        
	{
		if(cmd=="lui")  // one operand
		{
			opcode=15;
			register_map(result[1],rt);
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
			register_map(result[1],rt);
			stringstream ss;
			ss<<result[2];
			ss>>immediate;
			register_map(result[3],rs);
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

			register_map(result[1],rt);
			register_map(result[2],rs);
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
			register_map(result[1],rs);
			register_map(result[2],rt);
			int k;
			for(k=0;k<lp;k++)
			{

				if(result[3]==label_list[k].labelName)
				{
					// cout<<"solve label "<<label_list[k].labelName<<" to address "<<label_list[k].pos<<endl;
					int y=label_list[k].pos,x=PC+4;
					immediate=(y-x)/4;
					break;
				}
			}
			if(k==lp)
			{
				cout<<"Error!Find a word is neither label or KEYWORDS"<<endl;
			}
		}
		unsigned int x=immediate;
		x=x<<16;
		x=x>>16;
		bin+=x;
		bin+=rt<<16;
		bin+=rs<<21;
		bin+=opcode<<26;
		cout<<"********************************************************"<<endl;
		cout<<"Position:"<<PC<<" Check I type instruction:"<<endl;
		if(cmd!="lui")
			cout<<result[0]<<"\t"<<result[1]<<"\t"<<result[2]<<"\t"<<result[3]<<endl;
		else 
			cout<<result[0]<<"\t"<<result[1]<<"\t"<<result[2]<<endl;

		cout<<opcode;
		cout<<"\trs="<<rs;
		cout<<"\trt="<<rt;
		cout<<"\ti="<<immediate<<endl;
		cout<<"instruction=";
		printf("%08x\n",bin);
		cout<<"********************************************************"<<endl;
		
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
		cout<<"********************************************************"<<endl;
		cout<<"Position:"<<PC<<" Check J type instruction:"<<endl;
		cout<<result[0]<<"\t"<<result[1]<<endl;

		cout<<opcode;
		cout<<"\taddress="<<address<<endl;
		cout<<"instruction=";
		printf("%08x\n",bin);
		cout<<"********************************************************"<<endl;
	}
	else
	{
		cout<<"undefined word!"<<endl;
		return false;
	}

	PC+=4;
	return true;
}

int main()
{
	int choice=1;

	while(choice!=0)
	{
		cout<<endl;
		cout<<endl;
		cout<<endl;
		cout<<"---------------------------------------------------------------"<<endl;
		cout<<endl;
		cout<<"\t\tenter 0 to quit"<<endl;
		cout<<"\t\tassemble to binary line by line:enter 1"<<endl;
		cout<<"\t\tbinary to assemble line by line :enter 2"<<endl;
		cout<<"\t\tassemble to binary in file:enter 3"<<endl;
		cout<<"\t\tbinary to assemble in file : enter 4"<<endl;
		cout<<endl;
		cout<<"---------------------------------------------------------------"<<endl;
		cout<<"please enter your choice:";
		cin>>choice;
		if(choice==1)
		{
			single_asm_to_bin();
		}
		else if(choice==2)
		{
			single_bin_to_asm();
		}
		else if(choice==3)
		{
			asm_TO_bin();
		}
		else if(choice==4)
		{
			bin_TO_asm();
		}
	}
	cout<<"good bye~"<<endl;
}

void single_asm_to_bin()
{
	string input="welcome";
	unsigned int bin;
	while(input!="quit")
	{
		cout<<"input quit to quit"<<endl;
		cout<<"input an instruction:"<<endl;
		getline(cin,input);
		if(input=="quit")
			break;
		
		stringAnalyze(input,bin);
	}
}

int asm_TO_bin()
{
	string filename;
	fstream assembleFile;
	FILE * machineFile;

    // open input file
	cout<<"input the path of your file:";
	cin>>filename;
	assembleFile.open(filename.c_str(),ios::in);
	if(assembleFile.is_open()==false)
	{
		assembleFile.close();
		cout<<"can not open the file:"<<filename<<endl;
		return -1;
	}
    // create respond output file
	filename+=".bin";
	machineFile=fopen(filename.c_str(),"wb");
	if(machineFile==NULL)
	{
		cout<<"can not create the correspond machine code file!"<<endl;
		return -2;
	}

//scan for the first time to catch the label
	PC=0;
	cout<<"--------------------------------------------------------"<<endl;
	cout<<"\t\tScanning the label......"<<endl;
	cout<<"--------------------------------------------------------"<<endl;
	while(!assembleFile.eof())
	{
		string lineBuffer;
		char buffer[128];
		vector<string> seperate;

		assembleFile.getline(buffer,128);
		lineBuffer=buffer;
		if(scan_label(lineBuffer)==false)
		{
			cout<<"split line failed!"<<endl;
			continue;
		}
	}
	// cout<<endl;
	// cout<<endl;
	// cout<<endl;
	// cout<<"In file_convert function,checking the all label:"<<endl;
	// for(int k=0;k<lp;k++)
	// {
	// 	cout<<"label name="<<label_list[k].labelName<<" at "<<label_list[k].pos<<endl;
	// }
	// cout<<endl;
	// cout<<endl;
	// cout<<endl;
//scan for the second time to tranverse assemble to machinecode
	PC=0;
	assembleFile.clear();
	assembleFile.seekg(0);

	cout<<"--------------------------------------------------------"<<endl;
	cout<<"\t\tconvert the instruction......"<<endl;
	cout<<"--------------------------------------------------------"<<endl;
	while(!assembleFile.eof())
	{
		string lineBuffer;
		char buffer[128];
		unsigned int bin=0;

		assembleFile.getline(buffer,128);
		lineBuffer=buffer;
		if(stringAnalyze(lineBuffer,bin)==false)
		{
			cout<<"split line failed!"<<endl;
			continue;
		}
		else
		{
			char sbin[4];
			unsigned int temp=bin;

			for(int i=3;i>=0;i--)
			{
				sbin[i]=temp;
				temp=temp>>8;
			}
			for(int i=0;i<4;i++)
			{
				fwrite(&sbin[i],sizeof(char),1,machineFile);
			}
			// fwrite(&bin,sizeof(unsigned int),1,machineFile); // write 4 bytes=32bits
		}
	}
	fclose(machineFile);
	assembleFile.close();

	return 0;
}

int bin_TO_asm()
{
	FILE * machineFile;
	fstream assembleFile;
	string filename;

	cout<<"input your machinecode file:";
	cin>>filename;
	machineFile=fopen(filename.c_str(),"rb");
	if(machineFile==NULL)
	{
		cout<<"open "<<filename<<" failed!"<<endl;
		return -1;
	}
	filename+=".asm";
	assembleFile.open(filename.c_str(),ios::out);
	if(assembleFile.is_open()==false)
	{
		assembleFile.close();
		fclose(machineFile);
		cout<<"can not create the file:"<<filename<<endl;
		return -2;
	}
	PC=0;
	while(!feof(machineFile))
	{
		unsigned int bin=0;
		bool complete=true;
		unsigned char sbin[4];

		for(int i=0;i<4;i++)
		{
			if(fread(&sbin[i],sizeof(unsigned char),1,machineFile)!=1)
			{
				cout<<"read from machineFile failed!"<<endl;
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
			instruction+="\n";
			assembleFile<<instruction;
		}
	}
	fclose(machineFile);
	assembleFile.close();
}

bool binaryAnalyze(string & instruction,unsigned int &bin)
{
	unsigned int opcode=0,rs=0,rt=0,rd=0,shamt=0,funct=0;
	int immediate;
	unsigned int address;
	unsigned int temp=bin;
	string OP,V1,V2,V3;

	instruction+=to_string(PC)+":\t\t";
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

	address=bin<<6;
	address=address>>6;


// R type
	if(opcode==0)   
	{
		V1=bin_to_reg[rd];
		
		if(funct==0 || funct==2 || funct==3)   // shift
		{
			V2=bin_to_reg[rt];
			V3=std::to_string(shamt);
			switch(funct)
			{
				case 0:OP="sll";break;
				case 2:OP="srl";break;
				case 3:OP="sra";break;
				default:cout<<"invalid shift funct code!"<<endl;return false;
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
				default:cout<<"At Postion:"<<PC<<endl<<"invalid R type funct code !"<<endl;return false;
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

		cout<<"----------------------------------------------"<<endl;
		cout<<"\tbinary=";printf("%8x\n",bin);
		cout<<"\topcode\trs\trt\trd\tshamt\tfunct"<<endl;
		cout<<"\t"<<opcode<<"\t"<<rs<<"\t"<<rt<<"\t"<<rd<<"\t"<<shamt<<"\t"<<funct<<endl;
		cout<<"\tinstruction="<<instruction<<endl;
		cout<<"----------------------------------------------"<<endl;
	}
// J type
	else if(opcode==2 || opcode==3)  
	{
		if(opcode==2)
			OP="j";
		else
			OP="jal";

		V1=std::to_string(address*4);

		instruction+=OP+" "+V1;
		
		cout<<"----------------------------------------------"<<endl;
		cout<<"\tbinary=";printf("%8x\n",bin);
		cout<<"\topcode\taddress"<<endl;
		cout<<"\t"<<opcode<<"\t"<<address<<endl;
		cout<<"\tinstruction="<<instruction<<endl;
		cout<<"----------------------------------------------"<<endl;
	}
///   I type
	else                  
	{
		if(opcode==15)   // lui: two varialbes
		{
			OP="lui";
			V1=bin_to_reg[rt];
			V2=std::to_string(immediate);

			instruction+=OP+" "+V1+","+V2;
		}
		else if(opcode==35 || opcode==43)  // lw and sw: three varialbes but strange format
		{
			if(opcode==35)
				OP="lw";
			else
				OP="sw";

			V1=bin_to_reg[rt];
			V2=std::to_string(immediate);
			V3=bin_to_reg[rs];

			instruction+=OP+" "+V1+","+V2+"("+V3+")";
		}
		else   // three variables
		{
			V1=bin_to_reg[rt];
			V2=bin_to_reg[rs];
			if(opcode==4||opcode==5)
				V3=std::to_string(PC+4+immediate*4);
			else
				V3=std::to_string(immediate);
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
				default:cout<<"invalid I type opcode!"<<endl;return false;
			}

			instruction+=OP+" "+V1+","+V2+","+V3;
		}

		cout<<"----------------------------------------------"<<endl;
		cout<<"\tbinary=";printf("%8x\n",bin);
		cout<<"\topcode\trs\trt\timmediate"<<endl;
		cout<<"\t"<<opcode<<"\t"<<rs<<"\t"<<rt<<"\t"<<immediate<<endl;
		cout<<"\tinstruction="<<instruction<<endl;
		cout<<"----------------------------------------------"<<endl;
	}
	PC+=4;

	return true;
}

int single_bin_to_asm()
{
	string input="welcome";
	unsigned int bin;
	while(input!="quit")
	{
		cout<<"input quit to quit"<<endl;
		cout<<"input a binary instruction(decimal number):"<<endl;
		getline(cin,input);
		if(input=="quit")
			break;
		stringstream ss;
		ss<<input;
		ss>>bin;
		binaryAnalyze(input,bin);
		cout<<"instruction="<<input<<endl;
	}
}