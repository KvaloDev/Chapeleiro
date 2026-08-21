from pybricks.hubs import PrimeHub
from pybricks.messaging import BLERadio
from pybricks.tools import wait

CANAL_ANGULO = 8

# INICIALIZAÇÃO

hub = PrimeHub()

radio = BLERadio(
    broadcast_channel=CANAL_ANGULO
)

while not hub.imu.ready():
    wait(100)

hub.imu.reset_heading(0)

print("GIROSCOPIO PRONTO")

while True:

    angulo = hub.imu.heading()
    angulo_x10 = int(round(angulo * 10))
    radio.broadcast(angulo_x10)
    print(angulo)

    wait(100)