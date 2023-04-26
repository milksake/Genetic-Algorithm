import matplotlib.pyplot as plt
import csv

x = []
y = []

# Leer el archivo csv y extraer los datos de epoch y minAptitud
with open('datos.csv', 'r') as csvfile:
    plots = csv.reader(csvfile, delimiter=',')
    info = []
    # Leer las primeras tres líneas
    for i in range(3):
        line = next(plots)
        info.append(line[0]) # Agregar la línea al arreglo de info
    # Saltar la cuarta línea
    next(plots)
    # Leer las líneas restantes
    for row in plots:
        x.append(int(row[0]))
        y.append(float(row[1]))

# Graficar los datos y las líneas de información
plt.plot(x, y)
plt.text(0.1, 0.9, info[0], transform=plt.gca().transAxes)
plt.text(0.1, 0.85, info[1], transform=plt.gca().transAxes)
plt.text(0.1, 0.8, info[2], transform=plt.gca().transAxes)
plt.xlabel('Epoch')
plt.ylabel('MinAptitud')
plt.title('Traveling Salesman Problem using Genetic Algorithm')
plt.show()
