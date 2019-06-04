from flask import Flask, render_template, request, jsonify
from flask_socketio import SocketIO, send, emit
from datetime import datetime,timedelta
import json
from StringIO import StringIO
import sqlite3

@app.route("/data.json")
def TEST2():
	db=sqlite3.connect('mydb.sqlite')
	cur=db.cursor()
	x=cur.execute("SELECT * FROM smartmushroom where id % 100 =0 order by timestamp desc limit 500 ").fetchall()
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