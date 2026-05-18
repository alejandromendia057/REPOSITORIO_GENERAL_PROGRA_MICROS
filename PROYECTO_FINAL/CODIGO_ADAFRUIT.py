from Adafruit_IO import MQTTClient
import serial
import time
import threading

ADAFRUIT_IO_USERNAME = "Omar_Cabrera2005"
ADAFRUIT_IO_KEY = "aio_JYYJ805DPW9at1jKHwUYrtGg3wgw"
FEEDS = ["servo1", "servo2", "servo3", "servo4"]

ser = serial.Serial("COM4", 9600, timeout=1)
time.sleep(2)

def connected(client):
    print("Conectado!")
    for feed in FEEDS:
        print(f"Suscribiendo a {feed}")
        client.subscribe(feed)

def message(client, feed_id, payload):
    print(f"Adafruit → Arduino: {feed_id}:{payload}")
    msg = f"{feed_id}:{payload}\n"
    ser.write(msg.encode())

# HILO SEPARADO QUE LEE LO QUE ENVÍA EL ARDUINO
# Y LO PUBLICA DE VUELTA EN ADAFRUIT
def leer_arduino(client):
    while True:
        try:
            linea = ser.readline().decode().strip()
            if ':' in linea:
                # FORMATO ESPERADO: "Servo1:90"
                partes = linea.split(':')
                if len(partes) == 2:
                    feed  = partes[0]   # "Servo1"
                    valor = partes[1]   # "90"
                    if feed in FEEDS:
                        print(f"Arduino → Adafruit: {feed}:{valor}")
                        client.publish(feed, valor)
        except:
            pass
        time.sleep(0.05)

client = MQTTClient(ADAFRUIT_IO_USERNAME, ADAFRUIT_IO_KEY)
client.on_connect = connected
client.on_message = message
client.connect()

# ARRANCAR EL HILO DE LECTURA DEL ARDUINO
hilo = threading.Thread(target=leer_arduino, args=(client,), daemon=True)
hilo.start()

client.loop_blocking()