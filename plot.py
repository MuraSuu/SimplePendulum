import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
import subprocess

dataset = []
set_cord = [[], []]
cmd = './output.o'
process = subprocess.Popen(cmd, stdout=subprocess.PIPE)
i = 0

for line in process.stdout:
    temp = line
    if temp == b'End\n':
        i+=1
        dataset.append(set_cord)
        set_cord = [[], []]
        continue
    temp = str(line)
    temp = (temp[2:-3]).split()
    set_cord[0].append(float(temp[1])) #x1
    set_cord[1].append(float(temp[2])) #x2

fig, main_ax = plt.subplots()
main_ax.set_xlim(-5, 5)
main_ax.set_ylim(-4, 4)
N = len(dataset)
for i in range(0, N):
    main_ax.plot(dataset[i][0], dataset[i][1])
main_ax.set_xlabel('x1')
main_ax.set_ylabel('x2')
main_ax.set_title("phase space")
plt.show()
