#!/bin/bash
gunicorn -b 0.0.0.0:7200 -k gevent  --pid=app.pid server:app --capture-output --threads 1
