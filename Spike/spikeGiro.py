from pybricks.hubs import PrimeHub
from pybricks.messaging import BLERadio
from pybricks.tools import wait

hub = PrimeHub()

radio = BLERadio(
    broadcast_channel=8
)

while not hub.imu.ready():
    wait(100)

hub.imu.reset_heading(0)

print("VERIFICAÇAO")

while True:

    angulo = hub.imu.heading()
    angulo_x10 = int(round(angulo * 10))
    radio.broadcast(angulo_x10)
    print(angulo)

    wait(100)