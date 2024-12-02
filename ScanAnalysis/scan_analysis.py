import numpy as np
import matplotlib.pyplot as plt
from matplotlib.colors import LogNorm

fileName = "prepUni.txt"
#abre o arquivo e salva as duas primeiras linhas como strings
with open(fileName) as file:
    line1 = file.readline().strip()
    line2 = file.readline().strip()

#salva os passos em X e Y como variáveis float
stepX = float(line1.split("Step X = ")[1].strip().replace(',','.'))
stepY = float(line2.split("Step Y = ")[1].strip().replace(',','.'))

#salva os dados do arquivo em um array, pulando as duas primeiras linhas
arr = np.loadtxt(fileName, skiprows=2)

#numero de elementos em X e Y
nX = arr.shape[1]
nY = arr.shape[0]

#Reflete o array para imagem ser exibida na direção real do sensor
arr_reflected = arr[:, ::-1]

#dimensoes da figura
fig = plt.figure( figsize=(12,8) )
#plot em forma de matriz
#g = plt.imshow( arr, origin='lower', extent=( 0., stepX*(nX-1), 0., stepY*(nY-1)), cmap="coolwarm" )
g = plt.imshow( arr_reflected, origin='lower', extent=( -stepX*(nX-1), 0., 0., stepY*(nY-1)), cmap="coolwarm" )

fig.colorbar(g)

#plot em escala logaritmica
# plt.imshow( arr, origin='lower', extent=( 0., stepX*(nX-1), 0., stepY*(nY-1)), cmap="plasma", norm=LogNorm() )

plt.savefig( "mapa_log.png", bbox_inches='tight' )
plt.show()