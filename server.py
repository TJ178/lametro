from flask import Flask
from google.transit import gtfs_realtime_pb2
import datetime, threading, requests, jsonify


from secrets import API_KEY, API_PATH

headers = {
            "Authorization": API_KEY,
            "Accept": "application/json, application/json; charset=utf-8"
           }

app = Flask(__name__)

busdata = []

lock = threading.Lock()

@app.route('/', methods=['GET'])
def get_bus():
    print("hi")
    print(busdata)
    lock.acquire()
    data = busdata
    lock.release()
    return data

def getData():
    r = requests.get(API_PATH,headers=headers)
    feed = gtfs_realtime_pb2.FeedMessage()
    feed.ParseFromString(r.content)
    
    global busdata
    busdata = []

    lock.acquire()
    for entity in feed.entity:
        if entity.HasField('trip_update'):
            if entity.trip_update.trip.route_id == "2-13168":
                for stu in entity.trip_update.stop_time_update:
                    if stu.stop_sequence == 4 and stu.stop_id == "2442":      
                        busdata.append(stu.arrival.time)
    busdata.sort()
    threading.Timer(15, getData).start()
    lock.release()
    print(busdata[0])

with app.app_context():
   getData()

if __name__ == '__main__':
    getData()
    app.run()
