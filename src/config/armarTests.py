import random
import csv 


def posicionarJugadores(valorX, valorY, cantJugadores, yaPosicionados):
    posiciones = []
    for i in range(cantJugadores):
        pos_x = random.randint(1, valorX-1)
        pos_y = random.randint(1, valorY-1)
        while [pos_x, pos_y] in yaPosicionados:
            pos_x = random.randint(1, valorX-1)
            pos_y = random.randint(1, valorY-1)
        posiciones.append(pos_x)
        posiciones.append(pos_y)
        yaPosicionados.append([pos_x, pos_y])
    return posiciones

def armarTest(nombreTest, valorX, valorY, cantJugadores):
    
    banderaRojaX = 1
    banderaRojaY = random.randint(1, valorY-1)
    banderaAzulX = valorX - 1
    banderaAzulY = random.randint(1, valorY-1)
    
    data = [
        [valorX, valorY, cantJugadores],
        [banderaRojaX, banderaRojaY],
        [banderaAzulX, banderaAzulY]
    ]
    yaPosicionados = [[banderaRojaX, banderaRojaY], [banderaAzulX, banderaAzulY]]
    posicionesRojo = posicionarJugadores(valorX, valorY, cantJugadores, yaPosicionados)
    posicionesAzul = posicionarJugadores(valorX, valorY, cantJugadores, yaPosicionados)
    
    with open(nombreTest, "w", newline="") as f:
        writer = csv.writer(f, delimiter=" ")
        writer.writerows(data)
        writer.writerow(posicionesRojo)
        writer.writerow(posicionesAzul)


def crearTests(n):
    numero = 0
    for i in range(n):
        tamTablero = random.randint(5, 100)
        #tamTablero = 20000
        cota = int(tamTablero/2)
        armarTest(str(i) + ".csv", tamTablero, tamTablero, random.randint(1, min(cota,13))  )
        #//armarTest(str(numero) + ".csv", tamTablero, tamTablero, j)
        numero += 1

crearTests(1000)