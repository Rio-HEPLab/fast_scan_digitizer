import numpy as np
import matplotlib.pyplot as plt
from matplotlib.colors import LogNorm

fileName = "scanInvertido_1D.txt"
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

#eixo x do plot
Xvec = np.arange(0, nX, 1) * stepX

fig = plt.figure(figsize=(12,8))
plt.plot(Xvec, arr[0])
plt.plot(Xvec, arr[1])
plt.grid()

plt.savefig("Scan_1D")
plt.show()


