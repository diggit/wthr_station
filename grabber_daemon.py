#!/usr/bin/python3
# -*- coding: utf-8 -*-



#----------------------------------------
#	CONFIG

station_altitude=320 #in meters

ENC="utf-8"#for python conversions from chars into bytes, don't change :D

MINUTE_STEP=10#should be divider of 60, 10 min. are good..

ttyS_device="/dev/ttyUSB0"

BAUD=19200#baudrate, this must be same at sensor module program!!

UART_TIMEOUT=10#if station doesn't respond in this time, error will be reported

ERR_MSG_NORESPONSE="Stanice neodpovídá! Kontaktujte správce."#err message when station is not responding, will be on webpage

from os import uname

if uname()[1]=='raspberrypi':
	PATH="/home/pi/weather_station/web_app/"
else:
	PATH="/home/diggit/dev/dlouhodobka/web_app/"

#	END OF CONFIG
#----------------------------------------


M_lng=[31,28,31,30,31,30,31,31,30,31,30,31]


if(60%MINUTE_STEP):
	MINUTE_STEP=10

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
jsonly		- downloads values from station, used only in js file, no chart gererated or db access made

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

if "jsonly" in argv:
	jsonly=True
	GENchart=False
else:
	jsonly=False

if debug:
	print("DEBUG mode enabled!! warn: high verbosity ;-)")
	print("ttyS (serial) device:",ttyS_device)
	print("baud rate:",BAUD)
	print("minute step:",MINUTE_STEP)
	print("here we go")

elif dry and not jsonly:
	print("Dry run active, no db write will be done")


if not jsonly:
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

def check_status(RCVD):
	if debug:
		print("reading status from:",RCVD)

	output=""
	if(RCVD["I2C"]):
		output+="ERR i2c bus\\n"
	if(RCVD["ADT"]):
		output+="ERR ADT module\\n"
	if(RCVD["ADT0"]):
		output+="ERR ADT0 sensor\\n"
	if(RCVD["ADT1"]):
		output+="ERR ADT1 sensor\\n"	
	if(RCVD["ADT2"]):
		output+="ERR ADT2 sensor\\n"
	if(RCVD["BMP"]):
		output+="ERR BMP sensor\\n"
	if(RCVD["DHT"]):
		output+="ERR DHT sensor\\n"
	if(RCVD["GEN"]):
		output+="ERR GENERAL FAILURE\\n"

	if(len(output)==0):
		output="OK :)"
	return output;

def gen_actual_js(T,P,H,stamp,status):
	if debug:
		print("writing JS file")
	js=open(PATH+"actual.js","w")
	js.write("//dynamicaly generated, do not edit\n")
	js.write("var last_temp= %0.2f;\n" % T)
	js.write("var last_press= %i;\n" % P)
	js.write("var last_humi= %i;\n" % H)
	js.write("var stamp=\""+str(stamp)+"\";\n")
	js.write("var status=\""+status+"\";\n")
	js.close()
	if debug:
		print("write done!")

def gen_actual_raw(T,P,H,stamp):
	if debug:
		print("writing raw file")
	js=open(PATH+"actual","w")
	js.write("%0.1f\n" % T)
	js.write("%i\n" % P)
	js.write("%i\n" % H)
	js.write(str(stamp)+"\n")
	js.close()
	if debug:
		print("write done!")

#program flow continues below funcion defs
def gen_image(raw,Iname,Lcolor):
	
	raw.reverse()
	
	Xlist=[]
	allXlist=[]
	Ylist=[]
	
	time_jump=[]
	
	if debug:
		print(raw[0])
		print("first record, will be complete...")
	Ylist.append(round(raw[0][1],1))
	Xlist.append(str(raw[0][0])[5:16])
	allXlist.append(Xlist[0])
	
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
			allXlist.append(Tdate[5:16])
	
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
		
		
		#print("Lng:",len(Xlist),len(allXlist))
		for X in range(len(Xlist)):
			if (allXlist[X][-5:]in ["12:00","00:00"]):#at noon and midnight draw blue line
				ax.axvline(x=X,linewidth=0.75, linestyle='-', color="blue",alpha=0.75)
			elif (allXlist[X][-2:] == "00"):
				ax.axvline(x=X,linewidth=0.75, linestyle='-', color="grey",alpha=0.5)
			#print(allXlist[X])
		
		for T in time_jump:
			ax.axvspan(T,T+1,edgecolor="white",facecolor="red",alpha=0.4)

		savefig(PATH+"charts/"+Iname+".png",dpi=300,bottom=20,transparent=True)
		savefig(PATH+"charts/"+Iname+"-lowres.png",dpi=100,bottom=20,transparent=True)
	else:
		if debug:
			print("leaving, no chart generated")

def query_data():
	try:
		com=serial.Serial(ttyS_device,BAUD,timeout=UART_TIMEOUT)#open communication
		if debug:
			print("serial device opened")
	except:
		print("serial line could not be opened,\nany other program is reading or writing??\ndoes this device exist?")
		exit(1)
		
	if debug:
		print("requesting data")
	com.write(bytes("Q",ENC))#send request on data from measurement
	
	raw_received=""

	#skip all incomming messages, not containing  $ and !
	while not all(x in str(raw_received) for x in ["!","$"]):
		raw_received=com.readline()#receive them
		if(len(str(raw_received)[2:-3])==0):
			raise ConnectionError("No response")
		if debug:
			print("read:",str(raw_received)[2:-3],"len:",len(str(raw_received)[2:-3]))

	RCVD=str(raw_received)[2:-3].split(",")#split them into list, crop: " newline
	com.close()#close our comunication
	if debug:
		print("serial device closed")
	return RCVD

def decode_packet(RCVD):
	values={}#dictionary, key is property

	if debug:
			print("starting data conversion...")
	if RCVD[0]=='!' and RCVD[-1]=='$':#first char is "!" and last one "$", yeah it is our 'packet'
		#do some "translations"
		for i in range(2,len(RCVD)-2,2):#exclude first and last character, character(property) is every second -> step 2
			#print(RCVD[i])
			if("T" in RCVD[i] and len(RCVD[i])==2):#if T, temperature value follows
				values[RCVD[i]]=int(RCVD[i+1])/10 #convert it into number and divide by 10 (sensor measures with 0.1 accuracy but float is not used for ease of use)
				if debug or dry:
					print("temperature, key:",RCVD[i],"value=",values[RCVD[i]])
			elif("P" in RCVD[i] and len(RCVD[i])==2):#if P, pressure value follows
				values[RCVD[i]]=int(RCVD[i+1])/((1-station_altitude/44330)**5.255)/100 #formula from BMP085 datasheet
				if debug or dry:
					print("pressure, key:",RCVD[i],"value=",values[RCVD[i]])
			elif("H" in RCVD[i] and len(RCVD[i])==2):#if H, humdity value follows
				values[RCVD[i]]=int(RCVD[i+1])#convert it into num, that's all
				if debug or dry:
					print("humidity, key:",RCVD[i],"value=",values[RCVD[i]])
			else:
				values[RCVD[i]]=int(RCVD[i+1])#somethink unknown, just convert and save
		if debug:
			print(values)#print debug message
	else:
		if debug:
			print("wrong response from station, it was:",RCVD)
		gen_actual_js(0,0,0,strftime("%d.%m. %Y %H:%M"),ERR_MSG_NORESPONSE)
		exit(1)
	return values#dictionary of measured values
	

def db_access(values):
	try:
		conn = oursql.connect(user='station', passwd='trollface',db='weather', port=3306)#connect to mysql database
		if debug:
			print("connected to db!")
	except:
		print("unable to connect to mysql db, check for running daemon, database existence and table existence")
		exit(1)
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
	tmsg=curs.fetchall() #receive this

	curs.execute("SELECT stamp,P0 FROM data ORDER BY stamp DESC LIMIT 300;")#request last 300 lines of P0 (pressure)
	pmsg=curs.fetchall() #receive this

	curs.execute("SELECT stamp,H0 FROM data ORDER BY stamp DESC LIMIT 300;")#request last 300 lines of h0 (humidity)
	hmsg=curs.fetchall() #receive this

	if debug:
		print("received")
	curs.close()#destruct cursor
	conn.close()#disconnect from db
	if debug:
		print("db connection closed")
	return [tmsg,pmsg,hmsg]


if ttyS_device!="nodev":
	try:
		packet=query_data()
		data=decode_packet(packet)
	except ConnectionError as e:
		print("failed:",e.args[0])
	
	
else:
	print("No serial IO will be performed")
	data={}


if not jsonly:
	dbout=db_access(data)

	tmsg=dbout[0]
	pmsg=dbout[1]	
	hmsg=dbout[2]

	stamp=str(tmsg[0][0])

	#output generation
	if not dry:
		gen_actual_js(tmsg[0][1],pmsg[0][1],hmsg[0][1],stamp[8:10]+"."+stamp[5:7]+". "+stamp[0:4]+" "+stamp[11:16],check_status(data)) # DAY.MONTH. YEAR HOUR:MIN
	elif debug:
		print("no js file write!")
	
	if GENchart:
		if debug:
			print("loading matplotlib")
		try:
			from pylab import *
			import numpy as np
			from matplotlib.ticker import MultipleLocator, FormatStrFormatter
			if debug:
				print("loaded")
		except:
			print("ERROR, you must install matplotlib first, visit: http://matplotlib.org/\n or clone and build from GIT: https://github.com/matplotlib/matplotlib")
			exit(1)

		gen_image(tmsg,"temp","green")
		gen_image(pmsg,"pres","red")
		gen_image(hmsg,"humi","blue")

elif(not dry):
	if not data:
		print("no data received, was right device chosen? (-d nodev ???)")
		exit(1)
	stamp=strftime("%d.%m. %Y %H:%M")
	gen_actual_js(data["T0"],data["P0"],data["H0"],stamp,check_status(data)) # DAY.MONTH. YEAR HOUR:MIN
	gen_actual_raw(data["T0"],data["P0"],data["H0"],stamp) # DAY.MONTH. YEAR HOUR:MIN
else:
	print("no write done, js file remains untouched (dry ???)")

