import time
import paho.mqtt.client as mqtt
import firebase_admin
from firebase_admin import credentials, db

cred = credentials.Certificate("chave.json")
firebase_admin.initialize_app(cred, {
    "databaseURL": "https://fir-mqtt-9f86a-default-rtdb.firebaseio.com"
})

BROKER = "test.mosquitto.org"
TOPICO = "trabalho/iot/temperatura"

def on_connect(client, userdata, flags, rc, properties=None):
    print("Conectado ao MQTT")
    client.subscribe(TOPICO)

def on_message(client, userdata, msg):
    try:
        temp = float(msg.payload.decode())
    except ValueError:
        print("Mensagem ignorada:", msg.payload)
        return
    agora = int(time.time())
    print("Recebido:", temp)
    db.reference("sensor").set({"temperatura": temp, "timestamp": agora})
    db.reference("historico").push({"temperatura": temp, "timestamp": agora})

client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.on_connect = on_connect
client.on_message = on_message
client.connect(BROKER, 1883)
client.loop_forever()