#!/usr/bin/python3
# -*- coding: utf-8 -*-



#----------------------------------------
#	CONFIG

station_altitude=320 #in meters

ENC="utf-8"#for python conversions from chars into bytes, don't change :D

MINUTE_STEP=10#should be divider of 60, 10 min. are good..

ttyS_device="/dev/ttyUSB0"

BAUD=19200#baudrate, this must be same at sensor module program!!

from os import uname

if uname()[1]=='raspberrypi':
	PATH="/home/pi/weather_station/web_app/"
else:
	PATH="web_app/"

#	END OF CONFIG
#----------------------------------------

M_lng=[31,28,31,30,31,30,31,31,30,31,30,31]

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
nochart		- charts won't be generated (fast run, useful for debug)

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

if "nochart" in argv:
	GENchart=False
else:
	GENchart=True

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


if GENchart:
	if debug:
		print("loading matplotlib")
	try:
		from pylab import *
		if debug:
			print("loaded")
	except:
		print("ERROR, you must install matplotlib first, visit: http://matplotlib.org/\n or clone and build from GIT: https://github.com/matplotlib/matplotlib")
		exit(1)



try: 
	import oursql
	if debug:
		print("oursql lib loaded")
except:
	print("""oursql library must be installed,
sources ale located on launchpad: https://launchpad.net/oursql
choose correct version for python3! compile and install...""")
	exit(1)

if ttyS_device!="nodev":
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

def gen_actualjs(T,P,H,stamp):
	js=open(PATH+"actual.js","w")
	js.write("//dynamicaly generated, do not edit\n")
	js.write("var last_temp= %0.2f;\n" % T)
	js.write("var last_press= %0.2f;\n" % P)
	js.write("var last_humi= %i;\n" % H)
	js.write("var stamp=\""+str(stamp)+"\";\n")
	js.close()

#program flow continues below funcion defs
def gen_image(raw,Iname,Lcolor):
	
	#import matplotlib.pyplot as plt
	if GENchart:
		import numpy as np
		from matplotlib.ticker import MultipleLocator, FormatStrFormatter
	
	raw.reverse()
	
	Xlist=[]
	Ylist=[]
	
	time_jump=[]
	
	if debug:
		print(raw[0])
		print("first record, will be complete...")
	Ylist.append(round(raw[0][1],1))
	Xlist.append(str(raw[0][0])[5:16])
	
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
		else:#ok, not DUPLA, check that
			ERR=0
			#note when some part of date (month,day, hour, min) are same, it os OK, when it is increased by ONE, theme must be checked lower part to
			# eg DAY changed from 23 to 24, hours must be checked if they were changed from 23 to 0 and so on...
			if not (Tmonth==Pmonth or ((Tmonth-Pmonth==1 or (Tmonth==1 and Pmonth==12)) and Tday==1 and Pday==M_lng[Pmonth-1])):#NEG when are months OK
				ERR="MONTH"
			elif not (Tday==Pday or ((Tday-Pday==1 or (Tday==1 and Pday==M_lng[Pmonth-1])) and Thour==0 and Phour==23)):#NEG when are days OK
				ERR="DAY"
			elif not (Thour==Phour or ((Thour-Phour==1 or (Thour==0 and Phour==23)) and Pmin+MINUTE_STEP-60==Tmin )):#NEG when are hours OK (last cond. eg 54+10-60==4)
				ERR="HOUR"
			elif not (Pmin+MINUTE_STEP==Tmin or (Pmin==(60-MINUTE_STEP) and Tmin==0)):#NEG when mins are OK
				ERR="MINUTE"
			elif (Tmin%MINUTE_STEP):#if minutes are not aligned
				#ERR="MIN. ALIGN"
				if debug:
					print("WARNING - not aligned, but correct time step^^")
			
			if ERR:#wrong time steps
				if debug:
					print("date will be recorded ---------------^^ ERR DETECTED:"+ERR)
				Xlist.append(Tdate[5:16])
				##########################	
				time_jump.append(len(Xlist)-2)#in chart will be rectangle over this change
				
			#normal case whe date would be recorded
			elif(Tmonth!=Pmonth or Tday!=Pday or (Thour==12 and Tmin==0)):
				if debug:
					print("date will be recorded ---------------^^(OK)")
				Xlist.append(Tdate[5:16])
				
			else:#date won't be written
				if debug:
					print("no date, but will be recorded -------^^")
				Xlist.append("")
			Ylist.append(round(raw[Nline][1],1))
	
	#plotting follows
	if GENchart:
		if debug:
			print("generating plot")
		figure(figsize=(9,4), dpi=300)
		x = np.array(range(len(Xlist)))
		y = np.array(Ylist)
		my_xticks = Xlist
		xticks(x, my_xticks,rotation=90)
		ax = subplot(111)
		ax.tick_params(labelright=True)#Y labels on both sides (we've got wide chart)
		ax.yaxis.grid(True)#only Y axis grid
		y_formatter = matplotlib.ticker.ScalarFormatter(useOffset=False)
		ax.yaxis.set_major_formatter(y_formatter)
		plot(x,y,color=Lcolor,zorder=10)
		subplots_adjust(bottom=0.3,left=0.07,right=1-0.07)

		for X in range(len(Xlist)):
			if Xlist[X][-5:]in ["12:00","00:00"]:#at noon and midnight draw blue line
				ax.axvline(x=X,linewidth=0.75, linestyle='-', color="blue",alpha=0.75)
			#elif Xlist[X][-2:]=="00":#eg at 02:00 draw grey line
			#	ax.axvline(x=X,linewidth=0.75, linestyle='-', color="0.5" ,alpha=0.75)
			#elif Xlist[X][-5:]!="":#if date is showed and is not case of any above
			#	ax.axvline(x=X,linewidth=0.75, linestyle='-', color="red",alpha=0.75)
		
		for T in time_jump:
			ax.axvspan(T,T+1,edgecolor="white",facecolor="red",alpha=0.4)

		savefig(PATH+"charts/"+Iname+".png",dpi=300,bottom=20,transparent=True)
		savefig(PATH+"charts/"+Iname+"-lowres.png",dpi=100,bottom=20,transparent=True)
	else:
		if debug:
			print("leaving, no chart generated")

	
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
				values[RCVD[i]]=int(RCVD[i+1])/((1-station_altitude/44330)**5.255)/100 #formula from BMP085 datasheet
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
	

gen_actualjs(tmsg[-1][1],pmsg[-1][1],hmsg[-1][1],str(tmsg[0][0]))

gen_image(tmsg,"temp","green")
gen_image(pmsg,"pres","red")
gen_image(hmsg,"humi","blue")


