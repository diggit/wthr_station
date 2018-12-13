#!/usr/bin/python3
# -*- coding: utf-8 -*-



#----------------------------------------
#	CONFIG

station_altitude=320 #in meters

MINUTE_STEP=10#should be divider of 60, 10 min. are good..

PLOT_STEPS=300#how many values sould be in plotted

TIME_TICKS=["12:00","00:00"]#which times sould be printed onto X axis


ttyS_device="/dev/ttyUSB0"

BAUD=19200#baudrate, this must be same at sensor module program!!

UART_TIMEOUT=10#if station doesn't respond in this time, error will be reported

ERR_MSG_NORESPONSE="Stanice neodpovídá! Kontaktujte správce."#err message when station is not responding, will be on webpage

AVERAGING=20

WEIGHT_BASE=1.5

DECIMAL_PLACES=2 #vertical resolution of plots

GEN_HIRES=False #should we generate hi res plots too? (comsumes a lot of CPU time)

from os import uname

if uname()[-1]=='armv6l':
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

def print_help():
	print("""Welcome sky people!,
	This is simple python script for requesting and storing (in db) data from sensor module
	read more about project at: https://github.com/diggit/wthr_station


	debug		- Turns this script into mess generator :D, verbose enough
	insane		- debug will be even more verbose
	dry			- do not write into database
	nodb		- do not access database
	-d <device>	- Specify path to serial device, if not, default one is: /dev/ttyUSB0
	if device == 'nodev' no serial IO will be perf.
	chart		- generate charts (unless "nodb" specified)
	vartext		- generate plain text output from actual values
	varjs		- generate javascript file with variables

	For more features,visit github and place feature request""")

if "help" in argv:
	print_help()
	exit(0)

dry= "dry" in argv or "dryrun" in argv

if "-d" in argv:
	ttyS_device=argv[argv.index("-d")+1]#next argument after -d is device itself...

GENchart= "chart" in argv
GENtext= "vartext" in argv
GENjs= "varjs" in argv
debug= "debug" in argv
insane= "insane" in argv
dbaccess ="nodb" not in argv

#we won't need data from DB, do not access it at all...
if dry and not GENchart:
	nodb=True


if debug:
	print("DEBUG mode enabled!! warn: high verbosity ;-)")
if insane:
	print("INSANE mode enabled!! I'll tell you everything! }:]")
if insane or debug:
	print("CFG: ttyS (serial) device:",ttyS_device)
	print("CFG: baud rate:",BAUD)
	print("CFG: minute step:",MINUTE_STEP)
	print("CFG: here we go")

def printDBG(*tup):
	if debug or insane:
		for s in tup:
			print(s,end=" ")
		print("")

def printINS(*tup):
	if insane:
		for s in tup:
			print(s,end=" ")
		print("")

if ttyS_device=="nodev":
	printDBG("CFG: no device!")

if not dbaccess:
	printDBG("CFG: nodb -> no db access allowed, no charts will be generated!")
	GENchart=False #force chart off, we have no data to plot without db

if dry:
	printDBG("CFG: dry -> no db write will be done")
	# GENjs=False
	# GENtext=False
	# GENchart=False

if dbaccess:
	try:
		import oursql
		printDBG("IMP: oursql lib loaded")
	except:
		print("""IMP: ERR oursql library must be installed,
	sources ale located on launchpad: https://launchpad.net/oursql
	choose correct version for python3! compile and install...""")
		exit(1)

if ttyS_device!="nodev":
	try:
		import serial
		printDBG("IMP: serial lib loaded")
	except:
		print("""IMP: ERR serial library must be installed,
	should be easy to install from your distro repos
	if not, sources are here: http://pypi.python.org/pypi/pyserial
	BUT they must be converted for python3 before installing!!! or not?""")
		exit(1)

try:
	import datetime
	start_t=datetime.datetime.today()
	printDBG("IMP: datetime lib loaded")
except:
	print("""IMP: ERR can't include datetime library,
this is BAD because this lib is basic one,
check your python installation,
maybe your distro provides it in sep. package""")
	exit(1)

if GENchart:
	printDBG("IMP: loading pylab")
	try:
		import pylab as pl
		# import numpy as np
		# from matplotlib.ticker import ScalarFormatter
		printDBG("IMP: loaded")
	except:
		print("IMP: ERROR, you must install pylab first")
		exit(1)




def smoothen(data,delimiter):

	printDBG("SMTH: complete data set length",len(data))
	printINS("SMTH: content:\n",data)
	start=0
	end=0
	while len(data) not in [start,end]:#if start or end is set to end of data list
		if delimiter in data[start:]:#if there is any delimiter
			end=data[start:].index(delimiter)#find its index
			end+=start#but we are searching in cropped data set, we must add length of prefixed data to get real value
			printINS("SMTH: delimiter found at:",end)
		else:
			end=len(data)#if no more delimiters found, set end to end of data list

		if(start!=end):
			data=data[:start]+smoothen_raw(data[start:end])+data[end:]#replace part between delimiters by smoothed values
		start=end#set new possible start to place where we ended

		printINS("SMTH: start:",start)
		while start < len(data) and data[start] is delimiter:#skip delimiters until end
			printINS("SMTH: skipping delimiter at:",start)
			start+=1
		# raise Exception("breakpoint")

	return data


#weighted shifting average for smoothing curves
def smoothen_raw(raw,width=AVERAGING):
	printDBG("SMTHR: smoothed data length:",len(raw))
	if width<1 :
		return raw
	else:
		data=list(raw)#create copy, nothing more?
		extended=[]
		#add N copies of first record to start of list
		for _ in range(width):
			extended.append(data[0])

		extended+=raw

		#add N copies of last record to end of list
		for _ in range(width):
			extended.append(data[-1])


		for index in range(len(data)):
			average=0
			divider=0
			for chunk_index in range(width):
				average+=extended[width+index+chunk_index]* (WEIGHT_BASE**(width-chunk_index))
				average+=extended[width+index-chunk_index]* (WEIGHT_BASE**(width-chunk_index))
				divider+=2*(WEIGHT_BASE**(width-chunk_index))

			average/=divider
			data[index]=average
		del(extended)
		return data

#create html text with device status report
def gen_status_msg(condition, text):
	if(condition):
		return "<span class=\"status_fail\">"+text+": FAILED</span><br>"
	else:
		return "<span class=\"status_ok\">"+text+": OK</span><br>"

def gen_status(data_map):
	printDBG("STAT: reading status from:",data_map)

	status_names={"BHL":"BH Light sensor","I2C":"i2c BUS","ADT":"ADT module","ADT0":"ADT0 sensor","ADT1":"ADT1 sensor","ADT2":"ADT2 sensor","BMP":"BMP sensor","DHT":"DHT sensor","GEN":"GENERAL status"}

	output=""
	for key in status_names:
		if key in data_map:
			output+=gen_status_msg(data_map[key],status_names[key])
		else:
			output+=gen_status_msg(1,status_names[key]+" (unknown)")

	#if anything failed, set overal status to fail
	if ("status_fail" in output) or "FAIL" in data_map:
		output="<span class=\"status_fail\">ERR, contact admin!</span>\n"+output
	else:
		output="<span class=\"status_ok\">OK</span>\n"+output
	return output;

def gen_actual_js(T,P,H,L,stamp,status):
	printDBG("VAR: writing JS file")
	js=open(PATH+"actual.js","w")
	js.write("//dynamicaly generated, do not edit\n")
	js.write("var last_temp= %0.2f;\n" % T)
	js.write("var last_press= %i;\n" % P)
	js.write("var last_humi= %i;\n" % H)
	js.write("var last_light= %i;\n" % L)
	js.write("var stamp=\""+str(stamp)+"\";\n")
	js.write("var status=\""+status+"\";\n")
	js.close()
	printDBG("VAR: write done!")

def gen_actual_raw(T,P,H,L,stamp,status):
	printDBG("VAR: writing plain text file")
	js=open(PATH+"actual","w")
	js.write("%0.1f\n" % T)
	js.write("%i\n" % P)
	js.write("%i\n" % H)
	js.write("%i\n" % L)
	js.write(str(stamp)+"\n")
	js.write(status+"\n")
	js.close()
	printDBG("VAR: write done!")

#create chart from values from databse, with defined name and defined line color
def gen_image(data,Xcol,Ycol,ImgName,LineColor, logY = False):

	raw=[]
	for row in data:#copy only usable values
		raw.insert(0,[row[Xcol],row[Ycol]])
	# raw.reverse()

	XlistVisible=[]
	Xlist=[]
	Ylist=[]

	time_jump=[]

	matched_timestamps=0

	# if debug:
	# 	print(raw[0])
	# 	print("first record, will be complete...")
	# Ylist.insert(0,round(raw[0][1],DECIMAL_PLACES))
	# smoothYlist.insert(0,round(smooth[0][1],DECIMAL_PLACES))
	# Xlist.insert(0,str(raw[0][0])[5:16])
	# allXlist.insert(0,Xlist[0])

	stamp_t=start_t
	if(stamp_t.minute%MINUTE_STEP!=0):#if our actual step is off, fix it for next round
		printDBG("IMG: aligning timestamp")

	stamp_t-=datetime.timedelta(minutes=stamp_t.minute%MINUTE_STEP,seconds=stamp_t.second,microseconds=stamp_t.microsecond)#zerou out seconds

	for valueN in range(0,PLOT_STEPS):
		stamp=stamp_t.strftime("%Y-%m-%d %H:%M:%S")
		printINS(stamp,"--------------")


		match=False
		for N in range(0,len(raw)):
			# if debug:
				# print(raw[N][0],stamp_t)
				#print(raw[N][0].timetuple(),"\n",stamp_t.timetuple())
			if(raw[N][0] == stamp_t):
				printINS("IMG: timestamp match!")
				matched_timestamps+=1
				Ylist.insert(0,round(raw[N][1],DECIMAL_PLACES))
				#smoothYlist.insert(0,round(smooth[N][1],DECIMAL_PLACES))
				raw.pop(N)#this line of values won't be needed anymore
				match=True
				break
		if not match:#no matching timestamp found -> missing values
			Ylist.insert(0,pl.nan)
			printINS("IMG: no matching timestamp found, missing measurement!")

		if(stamp_t.strftime("%H:%M") in TIME_TICKS):#on specific hours
			XlistVisible.insert(0,stamp_t.strftime("%m/%d %H:%M"))# print date and time info on X axis, crop seconds
			printINS("IMG: visible timestamp recorded")
		else:
			XlistVisible.insert(0,"")#otherwise left blank
			printINS("IMG: visible timestamp ommited")
		Xlist.insert(0,stamp_t.strftime("%m/%d %H:%M"))

		stamp_t-=datetime.timedelta(minutes=10)

	if debug:
		print("IMG: we've ended with",matched_timestamps,"matched,",PLOT_STEPS-matched_timestamps,"missing and",len(raw),"unused time stamps")
		if insane:
			for i in raw:
				print(i[0])

	printDBG("IMG: smoothing curve...")
	smoothYlist=smoothen(Ylist,pl.nan)



	#plotting follows
	if GENchart:
		printDBG("IMG: generating plot")
		pl.figure(figsize=(9,4), dpi=300)
		printDBG("IMG: loading data")
		x = pl.array(range(len(XlistVisible)))
		y = pl.array(Ylist)
		smooth_y = pl.array(smoothYlist)

		printDBG("IMG: loading xticks")
		pl.xticks(x, XlistVisible,rotation=90)
		printDBG("IMG: setting plot properties")
		ax = pl.subplot(111)
		ax.tick_params(labelright=True)#Y labels on both sides (we've got wide chart)
		ax.tick_params(axis='y', colors='white')
		ax.tick_params(axis='x', colors='white')
		ax.yaxis.grid(True,color='white')#only Y axis grid
		ax.yaxis.set_major_formatter(pl.ScalarFormatter(useOffset=False))
		# ax.xaxis.grid(True, color="black")
		for n in ax.spines:
			ax.spines[n].set_color('white')

		#plot lines
		printDBG("IMG: plotting raw curve")
		pl.plot(x,y,color=LineColor,alpha=0.3,zorder=10)
		printDBG("IMG: plotting smooth curve")
		pl.plot(x,smooth_y,color=LineColor,zorder=10)
		if logY:
			pl.yscale('symlog', linthreshy=1)

		#adjust subplot
		pl.subplots_adjust(bottom=0.3,left=0.07,right=1-0.07)


		#print("IMG:Lng:",len(Xlist),len(allXlist))
		printDBG("IMG: adding X timestamps")
		for X in range(len(Xlist)):
			if (Xlist[X][-5:]in TIME_TICKS):
				ax.axvline(x=X,linewidth=0.75, linestyle='-', color="blue",alpha=0.75)
			elif (Xlist[X][-2:] == "00"):#every hour
				ax.axvline(x=X,linewidth=0.75, linestyle='-', color="grey",alpha=0.5)

		printDBG("IMG: hilighting missing measurements")

		ival=0
		while ival < len(Ylist):
			if Ylist[ival] is pl.nan:
				start=ival
				while(ival<len(Ylist) and Ylist[ival] is pl.nan):
					ival+=1
				printINS("IMG: hilighting missing from:",start,"to",ival)
				ax.axvspan(start,ival,facecolor="#FF0000",alpha=1)
			else:
				ival+=1

		if GEN_HIRES:
			if debug:
				print("IMG: saving high resolution file")
			pl.savefig(PATH+"charts/"+ImgName+".png",dpi=300,bottom=20,transparent=True)

		printDBG("IMG: saving low resolution file")
		pl.savefig(PATH+"charts/"+ImgName+"-lowres.png",dpi=100,bottom=20,transparent=True)
		printDBG("IMG: plot file done!")
	else:
		printDBG("IMG: leaving, no chart generated")

def query_data():
	try:
		com=serial.Serial(ttyS_device,BAUD,timeout=UART_TIMEOUT)#open communication
		printDBG("serial device opened")
	except:
		print("ERR: serial line could not be opened,\n\tany other program is reading or writing??\n\tdoes this device exist?")
		exit(1)

	if debug:
		print("requesting data...")
	com.write(b"Q")#send request on data from measurement

	raw_received=""

	#skip all incomming messages, not containing  $ and !
	while not all(x in str(raw_received) for x in ["!","$"]):
		raw_received=com.readline()#receive them
		if(len(str(raw_received)[2:-3])==0):
			raise Exception("No response")#in case of timeout...
		printDBG("read:",str(raw_received)[2:-3],"len:",len(str(raw_received)[2:-3]))

	com.close()#close our comunication
	if debug:
		print("serial device closed")
	return str(raw_received)[2:-3].split(",")#split them into list, crop: " newline

#list of values from device
def decode_packet(data):
	values={}#dictionary, key is property

	printDBG("starting data conversion...")
	if data[0]=='!' and data[-1]=='$':#first char is "!" and last one "$", yeah it is our 'packet'
		#do some "translations"
		for i in range(2,len(data)-2,2):#exclude first and last character, character(property) is every second -> step 2
			#print(data[i])
			if(data[i].startswith("T") and len(data[i])==2):#if "T?" where ? is sensor number, temperature value follows
				values[data[i]]=int(data[i+1])/10 #convert it into number and divide by 10 (sensor measures with 0.1 accuracy but float is not used for ease of use)
				if debug or dry:
					print("temperature, key:",data[i],"value=",values[data[i]])
			elif(data[i].startswith("L") and len(data[i])==2):#if "L?" where ? is sensor number, light value follows
				values[data[i]]=int(data[i+1])/2/1.2 #convert it into number (formula from datasheet), according to arduino lib, mode2 value must be additionaly divided by 2 (double resolution)
				if debug or dry:
					print("ambient light, key:",data[i],"value=",values[data[i]])
			elif(data[i].startswith("P") and len(data[i])==2):#if P, pressure value follows
				values[data[i]]=int(data[i+1])/((1-station_altitude/44330)**5.255)/100 #formula from BMP085 datasheet
				if debug or dry:
					print("pressure, key:",data[i],"value=",values[data[i]])
			elif(data[i].startswith("H") and len(data[i])==2):#if H, humdity value follows
				values[data[i]]=int(data[i+1])#convert it into num, that's all
				if debug or dry:
					print("humidity, key:",data[i],"value=",values[data[i]])
			else:
				values[data[i]]=int(data[i+1])#somethink unknown, just convert and save
		printDBG(values)#print debug message
	else:
		raise Exception("invalid response from station, it was:\n",data)
	return values#dictionary of measured values


def db_access(values,date_limit_str):
	try:
		conn = oursql.connect(user='station', passwd='trollface',db='weather', port=3306)#connect to mysql database
		printDBG("DB: connected!")
	except:
		print("DB: ERR- unable to connect to mysql db, check for running daemon, database existence and table existence")
		exit(1)

	curs = conn.cursor()#create cursor
	if not dry:
		curs.execute('INSERT INTO `data` (stamp,P0,T0,H0,L0) VALUES (?, ?, ?, ?, ?)',(start_t.strftime("%Y-%m-%d %H:%M:00"),values["P0"], values["T0"], values["H0"], values["L0"]))#save values into database
		printDBG("DB: data inserted into table")
	else:
		printDBG("DB: dry run! no db write done")

	if GENchart:
		# query="SELECT stamp,T0,P0,H0,L0 FROM data ORDER BY stamp DESC WHERE stamp > \'"+date_limit_str+"\' LIMIT "+str(PLOT_STEPS)+" ;"
		query="SELECT stamp,T0,P0,H0,L0 FROM data WHERE stamp > \'"+date_limit_str+"\' ORDER BY stamp DESC LIMIT "+str(PLOT_STEPS)+" ;"

		printDBG("DB: requesting data\nquery:",query)

		curs.execute(query)#request last 300 lines of T0 (temperature)
		db_data=curs.fetchall() #receive this

		printDBG("DB: data read")
	else:
		printDBG("DB: no data read")
		db_data=[]

	curs.close()#destruct cursor
	conn.close()#disconnect from db
	printDBG("DB: connection closed")
	return db_data



#here it RUNS!
data=[]
if ttyS_device!="nodev":
	try:
		packet=query_data()
		data=decode_packet(packet)
	except Exception as e:
		print("ERR: failed:",e.args[0])
		dry=True#because there is nothing to write to databse

else:
	print("No serial IO will be performed")

if dbaccess:
	date_limit=start_t-datetime.timedelta(minutes=start_t.minute%MINUTE_STEP+ MINUTE_STEP*PLOT_STEPS ,seconds=start_t.second,microseconds=start_t.microsecond)#zerou out s,us, align minutes, get furthest date, that could be displayed in chart
	if debug:
		print("DB: database query date limit:",date_limit)
	dbout=db_access(data,date_limit.strftime("%Y-%m-%dT%H:%M:%S.000"))

if GENchart:

	stamp=str(dbout[0][0])

	#output generation
	if GENjs:
		gen_actual_js(dbout[0][1],dbout[0][2],dbout[0][3],dbout[0][4],stamp[8:10]+"."+stamp[5:7]+". "+stamp[0:4]+" "+stamp[11:16],gen_status(data)) # DAY.MONTH. YEAR HOUR:MIN
	if GENtext:
		gen_actual_raw(dbout[0][1],dbout[0][2],dbout[0][3],dbout[0][4],stamp[8:10]+"."+stamp[5:7]+". "+stamp[0:4]+" "+stamp[11:16],gen_status(data)) # DAY.MONTH. YEAR HOUR:MIN


	gen_image(dbout,0,1,"temp","#00FF00")
	gen_image(dbout,0,2,"pres","#FFFF00")
	gen_image(dbout,0,3,"humi","#33CCFF")
	gen_image(dbout,0,4,"lght","#FFD700", True)

else: #otherwise create actual values directly from received data
	if not data:
		print("ERR: no data from device received, was right device chosen? (-d nodev ???)")
		T=P=H=0
	else:
		T=data["T0"]
		P=data["P0"]
		H=data["H0"]
		L=data["L0"]
	stamp=start_t.strftime("%d.%m. %Y %H:%M")

	if GENjs:
		gen_actual_js(T,P,H,L,stamp,gen_status(data)) # DAY.MONTH. YEAR HOUR:MIN
	if GENtext:
		gen_actual_raw(T,P,H,L,stamp,gen_status(data)) # DAY.MONTH. YEAR HOUR:MIN
