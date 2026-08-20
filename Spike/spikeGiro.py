from pybricks.hubs import PrimeHub
from pybricks.messaging import BLERadio
from pybricks.tools import wait


# ============================================
# CONFIGURAÇÃO
# ============================================

CANAL_ANGULO = 8


# ============================================
# INICIALIZAÇÃO
# ============================================

hub = PrimeHub()

radio = BLERadio(
    broadcast_channel=CANAL_ANGULO
)


# Espera a IMU ficar calibrada.
# Deixe o robô parado nesse momento.
while not hub.imu.ready():
    wait(100)


# Define a posição atual como 0° absoluto.
hub.imu.reset_heading(0)

print("GIROSCOPIO PRONTO")


# ============================================
# LOOP
# ============================================

while True:

    angulo = hub.imu.heading()

    # Enviamos em décimos de grau.
    #
    # 90.3°  -> 903
    # -45.7° -> -457
    #
    # Isso evita perder precisão usando int.
    angulo_x10 = int(round(angulo * 10))

    radio.broadcast(angulo_x10)

    print(angulo)

    # Pybricks transmite broadcasts
    # aproximadamente a cada 100 ms.
    wait(100)