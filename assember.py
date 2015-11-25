# -*- coding: utf-8 -*-
import re
from array import *

reg={"$zero":0,"$at":1,"$v0":2,"$v1":3,"$a0":4,"$a1":5,"$a2":6,"$a3":7,"$t0":8,"$t1":9,"$t2":10,"$t3":11,"$t4":12,"$t5":13,"$t6":14,"$t7":15,
	"$s0":16,"$s1":17,"$s2":18,"$s3":19,"$s4":20,"$s5":21,"$s6":22,"$s7":23,"$t8":24,"$t9":25,"$k0":26,"$k1":27,"$gp":28,"$sp":29,"$fp":30,"$ra":31,
	"$r0":0,"$r1":1,"$r2":2,"$r3":3,"$r4":4,"$r5":5,"$r6":6,"$r7":7,"$r8":8,"$r9":9,"$r10":10,"$r11":11,"$r12":12,"$r13":13,"$r14":14,"$r15":15,
	"$r16":16,"$r17":17,"$r18":18,"$r19":19,"$r20":20,"$r21":21,"$r22":22,"$r23":23,"$r24":24,"$r25":25,"$r26":26,"$r27":27,"$r28":28,"$r29":29,"$r30":30,"$r31":31,}

Rop={"add":32,"addu":33,"sub":34,"subu":35,"and":36,"or":37,"xor":38,"nor":39,"slt":42,"sltu":43,"sll":0,"srl":2,"sra":3,
	"sllv":4,"srlv":6,"srav":7,"jr":8,}  
Iop={"addi":8,"addiu":9,"andi":12,"ori":13,"xori":14,"lui":15,"lw":35,"sw":43,"beq":4,"bne":5,"slti":10,"sltiu":11,}
Jop={"j":2,"jal":3}

def main():
	while 1:
		label_list={}
		print("\nWelcome to use this assembler\n")
		print("--------------------------------\n\n")
		filename=input("input your filename.(input quit to quit):\n")
		if filename=="quit":
			break
		lineSet=[]
		with open(filename,'r') as infile:
			lines=infile.readlines()
			for line in lines:
				try:
					line=line.split('#')[0]      #remove the comment
				except:
					pass
				if line!='':
					lineSet.append(line)
		#scan for the label
		for i,line in enumerate(lineSet):
			print(i,line)
			words=[x.lower() for x in re.split(',|\s',line) if x!='']
			try:
				if ':' in words[0]:
					instr=words[0].strip(':')
					label_list[instr]=i
				elif words[1]==':':
					instr=words[0]
					label_list[instr]=i
			except:
				print('this line may less than 0 or 1 word\n')
		print(label_list)
		#scan the second time for convert to binary
		bin_array=array('I')
		for i,line in enumerate(lineSet):
			line=line.replace('(',' ')
			line=line.replace(')',' ')
			words=[x.lower() for x in re.split(',|\s',line) if x!='']
			try:                                 #remove the start 'label:'
				if ':' in words[0]:
					del words[0]
				elif words[1]==':':
					del words[0]
					del words[1]
			except:
				pass
			print('After remove start-label:')
			print(words)
			if len(words) <2:
				continue
			opcode=rs=rt=rd=shamt=funct=immediate=address=0
			binary=0
			if words[0] in Rop:
				opcode=0
				funct=Rop[words[0]]
				if words[0]=='jr':
					rd=rt=shamt=0;
					try:
						rs=reg[words[1]]
					except:
						print("No such register: "+words[1])
				elif words[0] in ['sll','srl','sra']:
					rd=reg[words[1]]
					rt=reg[words[2]]
					rs=0
					funct=Rop[words[0]]
					shamt=int(words[3])
				else:
					rd=reg[words[1]]
					rs=reg[words[2]]
					rt=reg[words[3]]
					shamt=0
					funct=Rop[words[0]]
				binary=binary+funct
				binary=binary+(shamt<<6)
				binary=binary+(rd<<11)
				binary=binary+(rt<<16)
				binary=binary+(rs<<21)
				binary=binary+(opcode<<26)
			elif words[0] in Iop:
				opcode=Iop[words[0]]
				if words[0]=='lui':
					rt=reg[words[0]]
					rs=0
					immediate=int(words[2])
				elif words[0] in ['lw','sw']:
					rt=reg[words[1]]
					rs=reg[words[3]]
					print(rt)
					print(rs)
					immediate=int(words[2])
				elif words[0] in ['beq','bne']:
					rs=reg[words[1]]
					rt=reg[words[2]]
					try:
						immediate=label_list[words[3]]-i-1
					except:
						print("no correspond label ! for "+words[3])
				else:
					rt=reg[words[1]]
					rs=reg[words[2]]
					immediate=int(words[3])
				binary=binary+immediate
				binary=binary+(rt<<16)
				binary=binary+(rs<<21)
				binary=binary+(opcode<<26)
			elif words[0] in Jop:
				opcode=Jop[words[0]]
				try:
					address=label_list[words[1]]
					print('adress=%s'%address)
				except:
					print("no correspond label ! for "+words[1])
					return False
				binary=binary+address
				binary=binary+(opcode<<26)
			temp='{0:032b}'.format(binary)
			print(hex(int(temp,2)))
			bin_array.append(int(temp,2))
		x_array=bytearray()
		for x in bin_array:
			print(hex(x))
			t4=x%256
			x=x>>8
			t3=x%256
			x=x>>8
			t2=x%256
			x=x>>8
			t1=x%256
			x_array.append(t1)
			x_array.append(t2)
			x_array.append(t3)
			x_array.append(t4)
		with open(filename+'.py.bin','wb') as outputfile:
			outputfile.write(x_array)
main()