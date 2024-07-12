import os
import serial
import logging
from logging.handlers import RotatingFileHandler
from flask import Flask, render_template
import threading
import json

app = Flask(__name__)

# 로그 파일 설정
if not os.path.exists('logs'):
    os.makedirs('logs')

handler = RotatingFileHandler('logs/sensor_data.log', maxBytes=10000, backupCount=1)
handler.setLevel(logging.INFO)
app.logger.addHandler(handler)

sensor_data = {
    'time': '',
    'temperature': 0.0,
    'humidity': 0.0,
    'pressure': 0.0,
    'gas': 0.0,
    'pm1_0': 0,
    'pm2_5': 0,
    'pm10': 0,
    'co2': 0.0,
    'nh3': 0.0,
    'nox': 0.0,
    'alcohol': 0.0,
    'benzene': 0.0,
    'smoke': 0.0,
    'airQuality': 0,
    'gasType': ''
}

def read_from_serial():
    global sensor_data
    ser = serial.Serial('/dev/ttyACM0', 9600)  # 시리얼 포트를 /dev/ttyACM0로 설정
    while True:
        try:
            line = ser.readline().decode('utf-8').strip()
            data = json.loads(line)
            sensor_data = {
                'time': data.get('time', ''),
                'temperature': float(data.get('temperature', 0.0)),
                'humidity': float(data.get('humidity', 0.0)),
                'pressure': float(data.get('pressure', 0.0)),
                'gas': float(data.get('gas', 0.0)),
                'pm1_0': int(data.get('pm1_0', 0)),
                'pm2_5': int(data.get('pm2_5', 0)),
                'pm10': int(data.get('pm10', 0)),
                'co2': float(data.get('co2', 0.0)),
                'nh3': float(data.get('nh3', 0.0)),
                'nox': float(data.get('nox', 0.0)),
                'alcohol': float(data.get('alcohol', 0.0)),
                'benzene': float(data.get('benzene', 0.0)),
                'smoke': float(data.get('smoke', 0.0)),
                'airQuality': int(data.get('airQuality', 0)),
                'gasType': data.get('gasType', '')
            }
            app.logger.info('Received data: %s', sensor_data)

            with open('logs/sensor_data.log', 'a') as f:
                f.write(f"Received data: {sensor_data}\n")

        except json.JSONDecodeError:
            app.logger.error('Error decoding JSON: %s', line)
        except Exception as e:
            app.logger.error('Error reading from serial: %s', e)

@app.route('/')
def index():
    return render_template('index.html', data=sensor_data)

if __name__ == '__main__':
    serial_thread = threading.Thread(target=read_from_serial)
    serial_thread.daemon = True
    serial_thread.start()
    app.run(host='0.0.0.0', port=5000)
