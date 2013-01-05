#!/usr/bin/python3
# -*- coding: utf-8 -*-

import oursql

import serial

from time import strftime

ENC="utf-8"

def gen_page(raw):
	raw.reverse()
	out=open("web_app/values.js","w")
	print(out.name)
	out.write("var myDataValues = [")
	for entry in raw:
		if((str(entry[0])[11:16]=="00:00")or(str(entry[0])[11:16]=="12:00")):#at midnight, at noon
			out.write("\n{date:\""+str(entry[0]))
		else:
			out.write("\n{date:\"")
		out.write("\", temp:"+str(round(entry[1],1))+"},")
	out.seek(out.tell()-1)
	out.write("\n];\n")
	out.close()



com=serial.Serial("/dev/ttyUSB0",19200)
com.write(bytes("Q",ENC))
S=com.readline()
RCVD=str(S)[2:-3].split(",")
#for i in RCVD:
	#print(i)
com.close()

values={}

if RCVD[0]=='!' and RCVD[-1]=='$':
	#do some "translations"
	for i in range(2,len(RCVD)-2,2):
		#print(RCVD[i])
		if("T" in RCVD[i]):
			values[RCVD[i]]=int(RCVD[i+1])/10
		elif("P" in RCVD[i]):
			values[RCVD[i]]=int(RCVD[i+1])/100
		else:
			values[RCVD[i]]=int(RCVD[i+1])
	print(values)

	conn = oursql.connect(user='station', passwd='trollface',db='weather', port=3306)
	curs = conn.cursor()
	curs.execute('INSERT INTO `data` (stamp,P0,T0,H0) VALUES (?, ?, ?, ?)',(strftime("%Y-%m-%d %H:%M:00"),values["P0"], values["T0"], values["H0"]))
	
	curs.execute("SELECT stamp,T0 FROM data ORDER BY stamp DESC LIMIT 300;")
	msg=curs.fetchall()
	gen_page(msg)

	curs.close()
	conn.close()


