# NUMPY and plots
import numpy as np
import seaborn as sns
import matplotlib
import matplotlib.pyplot as plt
matplotlib.rcParams['pdf.fonttype'] = 42
matplotlib.rcParams['ps.fonttype'] = 42
import sys




###############################################################################################
########################              Fitness Boxplot             #############################
###############################################################################################
# Open 2 folders and make the wilcoxon analisis Analisis_GANOSE and Analisis_GAOR
# Open the files and acquire the resultant fitness
folder = sys.argv[1]
gafile = open(folder+"/LCeofTest/Analisis_GASE/RawFitness.txt")
originalfile = open(folder+"/LCeofTest/Analisis_GAOR_NODFS/RawFitness.txt")

finalDataGA = {}
finalDataOr = {}

for line in gafile:
    line = line.strip().split(";")
    filteredData = []
    for fit in line[1:-1]:
        filteredData.append(float(fit))
    finalDataGA[line[0]] = filteredData

for line in originalfile:
    line = line.strip().split(";")
    filteredData = []
    for fit in line[1:-1]:
        filteredData.append(float(fit))
    finalDataOr[line[0]] = filteredData

gafile.close()
originalfile.close()

reorganicedData = []
for key in finalDataGA:
    reorganicedData.append([finalDataOr[key], finalDataGA[key]])



labels = ['CEA', '2-Step EA']
fig, (ax1, ax2, ax3) = plt.subplots(nrows=1, ncols=3, figsize=(12, 4))

# rectangular box plot
bplot1 = ax1.boxplot(reorganicedData[0],
                     vert=True,  # vertical box alignment
                     patch_artist=True,  # fill with color
                     labels=labels)  # will be used to label x-ticks
# ax1.set_title('Rectangular box plot')

# notch shape box plot
bplot2 = ax2.boxplot(reorganicedData[1],
                     vert=True,  # vertical box alignment
                     patch_artist=True,  # fill with color
                     labels=labels)  # will be used to label x-ticks
# ax2.set_title('Notched box plot')

# rectangular box plot
bplot3 = ax3.boxplot(reorganicedData[2],
                     vert=True,  # vertical box alignment
                     patch_artist=True,  # fill with color
                     labels=labels)  # will be used to label x-ticks
# ax3.set_title('Rectangular box plot')

# fill with colors
colors = ['pink', 'lightblue']
for bplot in (bplot1, bplot2, bplot3):
    for patch, color in zip(bplot['boxes'], colors):
        patch.set_facecolor(color)

# adding horizontal grid lines
for ax in [ax1, ax2, ax3]:
    ax.yaxis.grid(True)
    ax.set_ylabel('Fitness')

# plt.show()

# Save the plot as an image file (e.g., PNG, JPG, PDF, etc.)
plt.savefig(folder+"/Boxplot20.png")
plt.close()
