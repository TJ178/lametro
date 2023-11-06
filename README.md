# LA Metro Bus Tracker
A server and client combo to display the time of the next bus at the bus stop in front of my apartment.

A separate server is necessary to parse the full LA Metro API output, since the amount of data returned is large (>2MB).
The server regularly pings the LA Metro API (through Swiftly) for the most up-to-date bus information. 
It then parses out a specific bus line and stop, and offers that data up through an HTTP server.
This decreases the amount of data that the client device has to read from megabytes to only a few tens of bytes.

The client then pings the middleman server, and parses the output to get the time until the next bus!

## Setup
Clone the repository:
```
$ git clone https://github.com/TJ178/lametro.git
$ cd lametro
```

Create `secrets.py`:
- Obtain an API key from https://developer.metro.net/api/
- Create a file `server/secrets.py` that contains:
```Python
API_PATH = "https://api.goswift.ly/real-time/lametro/gtfs-rt-trip-updates"
API_KEY = "YOUR API KEY"
```

Open a venv:
```
$ python -m venv .venv
$ . .venv/bin/activate
```

Install the requirements:
```
$ pip install -r requirements.txt
```

Run the program:
```
$ bash start.sh
```

## Config
Right now, everything is hardcoded. However, it's relatively easy to change the values to any arbitrary station.

In `server/server.py`:
- Change `route_id` to the correct route ID.
- Change `stop_id` to the correct stop ID. You can find this in Google Maps by just searching for a specific stop.

In `client/client.ino`:
- Change the IP address of the server to the correct address.
