from flask import Flask, render_template, request, jsonify
from flask_socketio import SocketIO, send, emit
from datetime import datetime,timedelta
import json
from StringIO import StringIO
import sqlite3
##Target is Data from Browser to set Humidity,Temperature 
##State  is Data From Client(Wemos) to get Humidity,Temperature

app = Flask(__name__)
app.config['SECRET_KEY'] = 'secret!'
socketio = SocketIO(app)
##Defind Start Value##
targetH = 0
targetT = 0
switchT = '100'
switchH = '200'
Red = 0
Green = 0
Blue = 0
lastOffCooler = datetime.now()
lastSW=''
io = StringIO()
stateT = 0
stateH = 0
#Get data From Browser
@socketio.on('c2s')
def C2S(data):
	global targetT
	global targetH
	global Red
	global Green
	global Blue
	sdata=json.loads(data)
	targetT = float(sdata['T'])
	targetH = float(sdata['H'])
	Red = float(sdata['R'])
	Green = float(sdata['G'])
	Blue = float(sdata['B'])
	print "TARGET_T : %s"%(targetT)
	print "TARGET_H : %s"%(targetH)
	print "Red%03d,Green%03d,Blue%03d"%(Red,Green,Blue)

@app.route("/send")
def GET_DATA():
	return render_template('send.html')																#Render Slider Bar for Set Temperature amd Humidity

@app.route("/")
def hello():
	return render_template('index.html',name='SmartMushroomBox',message='Welcome EveryOne')			#Reder Pagr to Show Value 

@app.route("/data/<t>/<h>")																			#Get data From Client(Wemos)
def alarm(t,h):
	global Red
	global Green
	global Blue
	global stateT,stateH 
	log= str(datetime.now()) + "::temperature: %s *c,humidity: %s percent"%(t,h)
	print log
	#logT = "Temperature: %s"%(t)
	#logH = "Humidity   : %s"%(h)
	#print logH
	#print logT
	#print "Red%03d,Green%03d,Blue%03d"%(Red,Green,Blue)
	#return "RECIVED"
	stateT = t
	stateH = h
	write_file()
	writeDB()
	#SocketIO.emit('s2c',Red)
	return "%s,%s,%03d,%03d,%03d"%(SWcontrolT(t),SWcontrolH(h),Red,Green,Blue)
	
def write_file():
	global targetT ,targetH ,StateT ,stateH ,Red ,Green ,Blue
	print "write_file"
	with open("DATA.txt", "w+") as file:
		file.write(str(targetT))
		file.write(str(targetH))
		file.write(str(stateT))
		file.write(str(stateH))
		file.write(str(Red))
		file.write(str(Green))
		file.write(str(Blue))

	with open("DATA.txt", "r") as file:
		print file.read()
def SWcontrolT(StateT):
	global lastSW
	global lastOffCooler
	print "stateT: %s, targetT:%s"%(StateT,targetT)
	if targetT!='None':
		if(float (StateT)>float (targetT) and datetime.now()>(lastOffCooler+timedelta(minutes=1))):
			switchT="101"
			print "OnCooler"
		else:
			switchT="100"
			print "OffCooler"
	if(lastSW=="101" and switchT=="100"):
		lastOffCooler = datetime.now()
	lastSW=switchT
	#print "LastOff: %s"%lastOffCooler
	#print "DatetimeNow+delta: %s"%(lastOffCooler+timedelta(minutes=1))
	#print "DatetimeNow: %s"%datetime.now()

	return switchT

def SWcontrolH(StateH):
	global targetT ,targetH ,StateT ,stateH ,Red ,Green ,Blue
	print "stateH: %s, targetH:%s"%(StateH,targetH)
	if targetH !='None':
		if (float (StateH)<float (targetH)):
			switchH="201"
			print "OnPump"
		else:
			switchH="200"
			print "OffPump"

	return switchH
	
@app.route("/sliderbarTemp")
def sliderbarTemp():
	return render_template('sliderbarTemp.html')

@app.route("/sliderbarHumi")
def sliderbarHumi():
	return render_template('sliderbarHumi.html')

@app.route("/sliderbar")
def  sliderbar():
	return render_template('sliderbar.html')

@app.route("/naii")
def naii():
	return render_template('naii.html')
@app.route("/TEST1")
def TEST1():
	return render_template('TEST1.html')

@app.route("/data.json")
def TEST2():
	db=sqlite3.connect('mydb.sqlite')
	cur=db.cursor()
	x=cur.execute("SELECT * FROM smartmushroom").fetchall()
	y=x
	print "y: %s"%y
	db.close()
	data=[]
	for i in y:
		d={
			"id":i[0],
			"timestamp":i[1],
			"stateT":i[2],
			"stateH":i[3],
			"targetT":i[4],
			"targetH":i[5],
			"Red":i[6],
			"Green":i[7],
			"Blue":i[8],
		}
		data.append(d)

	return jsonify(data)

def writeDB():
	global stateT,stateH,targetT,targetH,Red,Green,Blue
	db=sqlite3.connect('mydb.sqlite')
	cur=db.cursor()
	command="INSERT INTO smartmushroom(stateT,stateH,targetT,targetH,Red,Green,Blue) VALUES (%s,%s,%s,%s,%s,%s,%s);"%(stateT,stateH,targetT,targetH,Red,Green,Blue)
	print command
	x=cur.execute(command)
	db.commit()
	x=cur.execute("SELECT * FROM smartmushroom")
	y=x.fetchall()
	db.close()
	
	return "y: %s"%y

#@app.teardown_appcontext
#def close_connection(exception):
#    db = getattr(g, '_database', None)
#    if db is not None:
#        db.close()


if __name__ == "__main__":
    #app.run(host='0.0.0.0',port=4999,debug=True)
    socketio.run(app)
#def writefile():
#	global targetT ,targetH ,StateT ,stateH ,Red ,Green ,Blue
#	file = open("DATA.txt", "w")
#	file.write(targetT)
#	file.write(targetH)
#	file.write(stateT)
#	file.write(stateH)
#	file.write(Red)
#	file.write(Green)
#	file.write(Blue)
#	file.close()
#	file = open('DATA', 'r')
#	print file.read()

