#!/usr/bin/python3
# -*- coding: utf-8 -*-



#----------------------------------------
#	CONFIG

ENC="utf-8"#for python conversions from chars into bytes, don't change :D

MINUTE_STEP=10#should be divider of 60, 10 min. are good..

ttyS_device="/dev/ttyUSB0"

BAUD=19200#baudrate, this must be same at sensor module program!!

#	END OF CONFIG
#----------------------------------------


#parse command line args
from sys import argv #basic lib, you've got it with python shipped

if "help" in argv:
	print("""Welcome sky people!,
This is simple python script for requesting and storing (in db) data from sensor module
read more about project at: https://github.com/diggit/wthr_station

your requested help, append parameters to activate function:
debug		- Turns this script into mess generator :D, verbose enough
dry, dryrun	- No db write will be performed
-d <device>	- Specify path to serial device, if not, default one is: /dev/ttyUSB0
if device == 'nodev' no serial IO will be perf.

For more features,visit github and place feature request""")
	exit(0)

if "dry" in argv or "dryrun" in argv:
	dry=True
else:
	dry=False

if "-d" in argv:
	ttyS_device=argv[argv.index("-d")+1]#next argument after -d is device itself...

if "debug" in argv:
	debug=True
else:
	debug=False


if debug:
	print("DEBUG mode enabled!! warn: high verbosity ;-)")
	if dry:
		print("Dry run active, no db write will be done")
	print("ttyS (serial) device:",ttyS_device)
	print("baud rate:",BAUD)
	print("minute step:",MINUTE_STEP)
	print("here we go")
elif dry:
	print("Dry run active, no db write will be done")




try: 
	import oursql
	if debug:
		print("oursql lib loaded")
except:
	print("""oursql library must be installed,
sources ale located on launchpad: https://launchpad.net/oursql
choose correct version for python3! compile and install...""")
	exit(1)

try:
	import serial
	if debug:
		print("serial lib loaded")
except:
	print("""serial library must be installed,
should be easy to install from your distro repos
if not, sources are here: http://pypi.python.org/pypi/pyserial
BUT they must be converted for python3 before installing!!! or not?""")
	exit(1)

try:
	from time import strftime
	if debug:
		print("part of time lib loaded")
except:
	print("""can't include time library,
this is BAD because this lib is basic one,
check your python installation,
maybe your distro provides it in sep. package""")
	exit(1)


if(60%MINUTE_STEP):
	MINUTE_STEP=10

#program flow continues below funcion defs

def gen_page(raw,F,VarName):
	if debug:
		print("generating output file")
	raw.reverse()
	if debug:
		print("opening file...")
	out=open("web_app/"+F,"w")
	if debug:
		print("output file name:",out.name)
		
	out.write("var "+VarName+" = [")
	
	
	out.write("\n{date:\""+str(raw[0][0])[5:16]+"\", val:"+str(round(raw[0][1],1))+"},")
	if debug:
		print(raw[0][0])
		print("first record, will be complete...")
	for Nline in range(1,len(raw)):
		if debug:
			print(raw[Nline][0])
		Pdate=str(raw[Nline-1][0])#date from previos record
		Tdate=str(raw[Nline][0])#this, actual date
		#print(Pdate[5:7],Pdate[8:10],Pdate[11:13],Pdate[14:16],"prev")
		#print(Tdate[5:7],Tdate[8:10],Tdate[11:13],Tdate[14:16],"this")
		
		Pmonth=int(Pdate[5:7])
		Pday=int(Pdate[8:10])
		Phour=int(Pdate[11:13])
		Pmin=int(Pdate[14:16])
		
		Tmonth=int(Tdate[5:7])
		Tday=int(Tdate[8:10])
		Thour=int(Tdate[11:13])
		Tmin=int(Tdate[14:16])
		
		if(Pdate==Tdate):
			if debug:
				print("^^^ duplicity detected!!! ^^^")
		else:# different month, different day, hour increase > 1, same hour and minute increase not eq MINUTE_STEP, hours increased (+1) and previous step minutes+MINUTE_STEP isn' equal to 60 (common reason for hour increase==1)        
			#any of those conditions decides about writing date information
			if(Tmonth!=Pmonth or Tday!=Pday or (Thour-Phour)>1 or (Thour==Phour and (Tmin-Pmin)!=MINUTE_STEP ) or ((Thour-Phour==1) and (Pmin+MINUTE_STEP!=60))):
				if debug:
					print("date will be recorded ---------------^^")
				out.write("\n{date:\""+Tdate[5:16])
			else:#date won't be written
				if debug:
					print("no date, but will be recorded -------^^")
				out.write("\n{date:\"")
			out.write("\", val:"+str(round(raw[Nline][1],1))+"},")
	out.seek(out.tell()-1)
	out.write("\n];\n")
	out.close()
	if debug:
		print("file closed!")

def gen_image(raw,Fpath):
	import matplotlib.pyplot as plt
	import numpy as np
	
	raw.reverse()
	
	Xlist=[]
	Ylist=[]
	
	if debug:
		print(raw[0])
		print("first record, will be complete...")
	for Nline in range(1,len(raw)):
		if debug:
			print(raw[Nline][0])
		Pdate=str(raw[Nline-1][0])#date from previos record
		Tdate=str(raw[Nline][0])#this, actual date
		#print(Pdate[5:7],Pdate[8:10],Pdate[11:13],Pdate[14:16],"prev")
		#print(Tdate[5:7],Tdate[8:10],Tdate[11:13],Tdate[14:16],"this")
		
		Pmonth=int(Pdate[5:7])
		Pday=int(Pdate[8:10])
		Phour=int(Pdate[11:13])
		Pmin=int(Pdate[14:16])
		
		Tmonth=int(Tdate[5:7])
		Tday=int(Tdate[8:10])
		Thour=int(Tdate[11:13])
		Tmin=int(Tdate[14:16])
		
		if(Pdate==Tdate):
			if debug:
				print("^^^ duplicity detected!!! ^^^")
		else:# different month, different day, hour increase > 1, same hour and minute increase not eq MINUTE_STEP, hours increased (+1) and previous step minutes+MINUTE_STEP isn' equal to 60 (common reason for hour increase==1)        
			#any of those conditions decides about writing date information
			if(Tmonth!=Pmonth or Tday!=Pday or (Thour-Phour)>1 or (Thour==Phour and (Tmin-Pmin)!=MINUTE_STEP ) or ((Thour-Phour==1) and (Pmin+MINUTE_STEP!=60))):
				if debug:
					print("date will be recorded ---------------^^")
				Xlist.append(Tdate[5:16])
			else:#date won't be written
				if debug:
					print("no date, but will be recorded -------^^")
				Xlist.append("")
			Ylist.append(round(raw[Nline][1],1))
	
	
	x = np.array(range(len(Xlist)))
	y = np.array(Ylist)
	my_xticks = Xlist
	plt.xticks(x, my_xticks,rotation=45)
	plt.plot(x, y)
	plt.grid(True)

	figure = plt.gcf() # get current figure
	figure.set_size_inches(9, 3)
	figure.set_dpi(300)
	figure.set_alpha(0)#doesnt work----
	plt.savefig(Fpath, transparent = False,dpi=300)
	
if ttyS_device!="nodev":
	try:
		com=serial.Serial(ttyS_device,BAUD)#open communication
		if debug:
			print("serial device opened")
	except:
		print("serial line could not be opened,\nany other program is reading or writing??\ndoes this device exist?")
		exit(1)
		
	if debug:
		print("requesting data")
	com.write(bytes("Q",ENC))#send request on data from measurement
	raw_recieved=com.readline()#recieve them
	if debug:
		print("recieved:",raw_recieved)
	RCVD=str(raw_recieved)[2:-3].split(",")#split them into list, crop: " newline
	com.close()#close our comunication
	if debug:
		print("serial device closed")


	values={}#dictionary, key is property

	if debug:
			print("starting data conversion...")
	if RCVD[0]=='!' and RCVD[-1]=='$':#first char is "!" and last one "$", yeah it is our 'packet'
		#do some "translations"
		for i in range(2,len(RCVD)-2,2):#exclude first and last character, character(property) is every second -> step 2
			#print(RCVD[i])
			if("T" in RCVD[i]):#if T, temperature value follows
				values[RCVD[i]]=int(RCVD[i+1])/10 #convert it into number and divide by 10 (sensor measures with 0.1 accuracy but float is not used for ease of use)
				if debug or dry:
					print("temperature, key:",RCVD[i],"value=",values[RCVD[i]])
			elif("P" in RCVD[i]):#if P, pressure value follows
				values[RCVD[i]]=int(RCVD[i+1])/100 #convert it into num and then divide by 100, because it is in pascals (Pa) and we want hecto pascals (hPa)
				if debug or dry:
					print("pressure, key:",RCVD[i],"value=",values[RCVD[i]])
			elif("H" in RCVD[i]):#if H, humdity value follows
				values[RCVD[i]]=int(RCVD[i+1])#convert it into num, that's all
				if debug or dry:
					print("humidity, key:",RCVD[i],"value=",values[RCVD[i]])
			else:
				values[RCVD[i]]=int(RCVD[i+1])#somethink unknown, just convert and save
		if debug:
			print(values)#print debug message
else:
	print("No serial IO will be performed")



	try:
		conn = oursql.connect(user='station', passwd='trollface',db='weather', port=3306)#connect to mysql database
		if debug:
			print("connected to db!")
	except:
		print("unable to connect to mysql db, check for running daemon, database existence and table existence")
	curs = conn.cursor()#create cursor
	if not dry:
		curs.execute('INSERT INTO `data` (stamp,P0,T0,H0) VALUES (?, ?, ?, ?)',(strftime("%Y-%m-%d %H:%M:00"),values["P0"], values["T0"], values["H0"]))#save values into database
		if debug:
			print("data inserted into table")
	elif debug:
		print("dry run! no db write done")
	
	if debug:
		print("requesting data from db")
	
	curs.execute("SELECT stamp,T0 FROM data ORDER BY stamp DESC LIMIT 300;")#request last 300 lines of T0 (temperature)
	tmsg=curs.fetchall() #recieve this
	
	curs.execute("SELECT stamp,P0 FROM data ORDER BY stamp DESC LIMIT 300;")#request last 300 lines of T0 (temperature)
	pmsg=curs.fetchall() #recieve this
	
	curs.execute("SELECT stamp,H0 FROM data ORDER BY stamp DESC LIMIT 300;")#request last 300 lines of T0 (temperature)
	hmsg=curs.fetchall() #recieve this
	
	if debug:
		print("recieved")
	curs.close()#destruct cursor
	conn.close()#disconnect from db
	if debug:
		print("db connection closed")
	

	#gen_page(tmsg,"temp.js","tempVals")#generate file with JS array of those values for web
	#gen_page(pmsg,"pres.js","presVals")#generate file with JS array of those values for web
	#gen_page(hmsg,"humi.js","humiVals")#generate file with JS array of those values for web
	gen_image(tmsg,"temp.png")
