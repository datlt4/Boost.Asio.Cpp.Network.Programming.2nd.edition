# python3 -m pip install websocket-client
import os
from websocket import create_connection
import time
from datetime import datetime
ip = "127.0.0.1"
port = "8673"
while 1:
    try:
        ws = create_connection(f"ws://{ip}:{port}")
        print(f"connection to ws://{ip}:{port} was established")
        while 1:
            data = ws.recv()
            if data == "reboot-jetson":
                print("reboot-jetson", datetime.now())
                os.system("/sbin/reboot 0")
            if data == "shutdown-jetson":
                print("shutdown-jetson", datetime.now())
                os.system("/sbin/shutdown -h now")
    except OSError as e:
        print(e, datetime.now())
        time.sleep(1)

