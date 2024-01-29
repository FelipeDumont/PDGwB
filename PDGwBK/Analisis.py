import io
import os

# NUMPY and plots
import numpy as np
import seaborn as sns
import matplotlib
import matplotlib.pyplot as plt
matplotlib.rcParams['pdf.fonttype'] = 42
matplotlib.rcParams['ps.fonttype'] = 42

# C Call
from ctypes import CDLL
from ctypes import c_size_t, c_double, c_int

# Random
import random

# File control
import glob
import sys

# Get excecution time !
import time

############################# Functions !  #############################

def CreateSeeds(sequenceSeed, seedQuantity):
  random.seed(sequenceSeed)
  unique_seeds = set()
  while len(unique_seeds) < seedQuantity:
      seed = random.randint(0, 2**32 - 1)
      unique_seeds.add(seed)

  seeds = list(unique_seeds)
  # print(seeds)
  return seeds

def GenerateHeatmap(data, titleLabel, XLabel, YLabel, fileName, offset):
  # Create a heatmap with a specific color map
  ax = sns.heatmap(data, cmap='Greys')  # You can replace 'coolwarm' with your preferred colormap
  ax.invert_yaxis()

  # Adding title and labels (optional)
  plt.title(titleLabel)
  plt.xlabel(XLabel)
  plt.ylabel(YLabel)

  # Get current tick tables
  x_tick = ax.get_xticklabels()
  y_tick = ax.get_yticklabels()
  
  for x in x_tick:
     newX = int(x.get_text())
     x.set_text(newX - offset)
  for y in y_tick:
     newY = int(y.get_text())
     y.set_text(newY - offset)
  
  ax.set_xticklabels(x_tick)
  ax.set_yticklabels(y_tick)
  
  # Save the plot as an image file (e.g., PNG, JPG, PDF, etc.)
  plt.savefig(fileName + ".png")
  plt.close()

  # Show the plot
  # plt.show()

def linePlot(data, titleLabel, XLabel, YLabel, verticalLines, verticalLabels, fileName):
  # Create the plot
  plt.figure(figsize=(10, 6))

  # Plot each line
  for i in range(1,len(data)):
    plt.plot(data[0], data[i][0], label=data[i][1], color=data[i][2])

  # Add labels and title
  plt.xlabel(XLabel)
  plt.ylabel(YLabel)
  plt.title(titleLabel)

  # Add a legend
  plt.legend()
  if(len(verticalLines) > 0):
    for xvalue in range(len(verticalLines)):
        plt.axvline(x=verticalLines[xvalue], color='red', linestyle='--', label=verticalLabels[xvalue])

  # Save the plot as an image file (e.g., PNG, JPG, PDF, etc.)
  plt.savefig(fileName + ".png")
  plt.close()
  # Show the plot
  # plt.show()

def runGeneratorOriginal(seed, useDFS):
    libfile = ''
    # Get the current directory of the Python script
    script_dir = os.path.dirname(os.path.abspath(__file__))
    libfile = os.path.join(script_dir, 'Original/pdgwbSO')
    mylib = CDLL(libfile)

    # Size will be the number of rooms * 2(double the size in case of bad resutls) * 6 (id,x,y,sonL,sonD,sonR, KeyID)
    nV = int(sys.argv[2])
    numKeys = int(sys.argv[3])
    numLocks = int(sys.argv[4])
    lCoefficient = float(sys.argv[5])
    neededLocks = float(sys.argv[6])
    size = nV * 2 * 7
    R = np.full(size, -1.0)

    if size < 18: # to pass all the variables !
        size = 18
    R = np.full(size, -1.0)

    # Analisis np 3 (min, max. avg) * 100 (quantity of generations) + 5 (data)
    analisisSize = (3 * 100) + 5 
    AR = np.full(analisisSize, -1.0)

    # Defining prototypes to send data
    mylib.CreateDungeons.argtypes = [np.ctypeslib.ndpointer(dtype=np.float64, shape=(size,)), np.ctypeslib.ndpointer(dtype=np.float64, shape=(analisisSize,))]

    # Paper settings and problem settings
    R[0] = 100 # Generations
    R[1] = 10  # Deph
    R[2] = 100 # population
    R[3] = nV
    R[4] = numKeys
    R[5] = numLocks
    R[6] = lCoefficient
    R[7] = seed 
    R[8] = 1
    
    if(useDFS == False):
        R[9] = 1
    mylib.CreateDungeons(R, AR)
    return R, AR

def runGenerator(seed, useSE, sCoef, lCoefW, sCoefW):
    libfile = ''
    # Get the current directory of the Python script
    script_dir = os.path.dirname(os.path.abspath(__file__))

    # Specify the relative path to the shared library
    libfile = os.path.join(script_dir, '2StepEA/pdgwbSO')
    mylib = CDLL(libfile)

    # First variable extracted manually to create the initial Array to obtain the dungeons !
    # Size will be the number of rooms * 2(double the size in case of bad resutls) * 6 (id,x,y,sonL,sonD,sonR, KeyID) + 2 (last room Position)
    nV = int(sys.argv[2]) # Number of rooms
    maxRange = nV * 10 # Considered for testing and generation of extra rooms in the first generations
    size = (maxRange * 7) + 2

    if size < 22: # to pass all the variables !
        size = 22
    R = np.full(size, -1.0)

    # print(sys.argv[7] )
    # Pass the values by args
    if len(sys.argv) > 17:
        for i in range(1, 7):
            R[i - 1] = float(sys.argv[i]) 
    if sys.argv[7] == "true":
        R[6] = 1
    else:
        R[6] = 0
    # Force the analisis !
    R[7] = 1
        
    for i in range(9,18):
        R[i - 1] = float(sys.argv[i]) 

    if(useSE == False):
       # print("removing the Symetry ... ", R[14])
       R[14] = 0 # Se chance
    
    R[17] = sCoef
    R[18] = lCoefW
    R[19] = sCoefW

    displayFlag = sys.argv[-1] == "true"
    # Analisis format
    analisisSize = (3 * int(sys.argv[9]) + 3 * int(sys.argv[10])) + 5 
    AR = np.full(analisisSize, -1.0)

    # Assign the seed
    R[0] = seed
    # C-type corresponding to numpy array
    mylib.CreateDungeons.argtypes = [np.ctypeslib.ndpointer(dtype=np.float64, shape=(size,)), np.ctypeslib.ndpointer(dtype=np.float64, shape=(analisisSize,)), c_int]
    mylib.CreateDungeons(R, AR, size)
    
    return R, AR

############################# MAIN  #############################
N = int(sys.argv[21])
mainSeed = 42
seeds = CreateSeeds(mainSeed, N)
heatmapSizes = {15:19, 20:22, 25: 27, 30: 25, 100: 61, 500: 251}

# Generations form and BK
genForm = int(sys.argv[9])
genBK = int(sys.argv[10])

numVertices = int(sys.argv[2])
numKeys = int(sys.argv[3])
numLocks = int(sys.argv[4])
lCoefficient = float(sys.argv[5])
neededLocks = float(sys.argv[6])
problemName = str(numVertices)+"_"+str(numKeys)+"_"+str(numLocks)+"_"+str(lCoefficient)+"_"+str(neededLocks)
useSE = True
useOriginal = False
useDFS = True
mode = float(sys.argv[19]) # 0 -> SE, 1 -> NOSE, 2 -> Original
currentFolder = sys.argv[20]

# Symmetry and the fitness function weights
# 0 means not used and 1 means used
# the study of the use of the symmetry is not finished yet but you can continue throu this part !
# Good luck with this !
sCoef = 0
lCoefW = 1
sCoefW = 0

if(mode == 1):
    useSE = False
elif(mode == 2):
    useSE = False
    useOriginal = True
elif(mode == 3):
    useSE = False
    useOriginal = True
    useDFS = False
currentSize = heatmapSizes[numVertices]
currentSizeCenter = currentSize//2

### DATA
allR = []
allAR = []
runTime = []
lastFitness = []
### RUN All, for now just use the data directly
### it would be better to save them and then work throu this data
for seed in seeds:
    start_time = time.time()
    if useOriginal == False:
        R, AR = runGenerator(seed, useSE, sCoef, lCoefW, sCoefW)
        lastFitness.append(AR[(genForm + genBK-1)*3])
    else:
        R, AR = runGeneratorOriginal(seed, useDFS)
        lastFitness.append(AR[(100-1)*3])
    end_time = time.time()
    runTime.append(end_time - start_time)
    
    allR.append(R)
    allAR.append(AR)

nRooms = int(sys.argv[2])
shape = (currentSize, currentSize)
heatMatrix = np.zeros(shape)

# Generate the Heatmap
c = 0
for R in allR:
    roomPositions = {}
    for i in range(nRooms*2):
        base = i * 7
        if(int(R[base]) == -1):
            continue
        roomPositions[i] = [int(R[base+ 1]),int(R[base+2])]
        
    # Save the rroms in the np array
    newHeat = np.zeros(shape)
    for k in roomPositions:
        if roomPositions[k][1] + currentSizeCenter > currentSize - 1:
            print("!!!!!!!!!!!  Errror in position X Pos [ {},{}]".format((roomPositions[k][1] + currentSizeCenter), roomPositions[k][0] + currentSizeCenter))
        elif roomPositions[k][0] + currentSizeCenter > currentSize - 1:
            print("!!!!!!!!!!!  Errror in position Y Pos [ {},{}]".format((roomPositions[k][1] + currentSizeCenter), roomPositions[k][0] + currentSizeCenter))
        else:
            newHeat[roomPositions[k][1] + currentSizeCenter ][roomPositions[k][0] + currentSizeCenter ] = 1
    heatMatrix = heatMatrix + newHeat
    
    c+=1

heatMatrix = heatMatrix / N
GenerateHeatmap(heatMatrix, '', '', '', currentFolder+ '/Heatmap' +problemName, currentSizeCenter)


# Generate Convergence line matrix
minF = []
maxF = []
avgF = []
totalSize = 0
ARSUM = allAR[0].copy()
for ID in range(1, N):
    
    ARSUM += allAR[ID]
ARSUM /= N

if(useOriginal == False):
    totalSize = genForm + genBK
    for id in range(genForm + genBK):
        currentID = id * 3
        minF.append(ARSUM[currentID + 0])
        maxF.append(ARSUM[currentID + 1])
        avgF.append(ARSUM[currentID + 2])
    x = np.linspace(0, genForm + genBK, genForm + genBK)  # genForm + genBK points from 0 to genForm + genBK
    data = [x, [minF,'Min Fitness', 'blue'], [maxF,'Max Fitness', 'green'], [avgF,'Avg. Fitness', 'red']]
    linePlot(data, 'Convergence', 'Generations', 'Fitness', [genForm+1],['Second Step'], currentFolder + '/LineG1' +problemName)

    x = np.linspace(0, genForm , genForm)  # genForm + genBK points from 0 to genForm + genBK
    data = [x, [minF[:genForm],'Min Fitness', 'blue'], [maxF[:genForm],'Max Fitness', 'green'], [avgF[:genForm],'Avg. Fitness', 'red']]
    linePlot(data, 'Convergence First Step', 'Generations', 'Fitness', [],[], currentFolder + '/LineG2' +problemName)
    
    x = np.linspace(0, genBK , genBK)  # genForm + genBK points from 0 to genForm + genBK
    data = [x, [minF[genForm:],'Min Fitness', 'blue'], [maxF[genForm:],'Max Fitness', 'green'], [avgF[genForm:],'Avg. Fitness', 'red']]
    linePlot(data, 'Line Graph Title 3', 'Generations', 'Fitness', [],[],currentFolder + '/LineG3' +problemName)

else:
    totalSize = 100
    for id in range(100):
        currentID = id * 3
        minF.append(ARSUM[currentID + 0])
        maxF.append(ARSUM[currentID + 1])
        avgF.append(ARSUM[currentID + 2])
    x = np.linspace(0, 100, 100)  # genForm + genBK points from 0 to genForm + genBK
    data = [x, [minF,'Min Fitness', 'blue'], [maxF,'Max Fitness', 'green'], [avgF,'Avg. Fitness', 'red']]
    linePlot(data, 'Convergence', 'Generations', 'Fitness', [],[], currentFolder + '/LineG1' +problemName)

# Finally Save the resulting data, still missing the whole data ...
rooms = []
keys = []
barriers = []
lcoef = []
nLocks = []
for AR in allAR:
   rooms.append(AR[-5])
   keys.append(AR[-4])
   barriers.append(AR[-3])
   lcoef.append(AR[-2])
   nLocks.append(AR[-1])

file = open(currentFolder + "/Results.txt","a")
file.write("{}_{}_{}_{}_{};".format(numVertices, numKeys, numLocks, lCoefficient, neededLocks))

mean = np.average(rooms)
std_deviation = np.std(rooms)
file.write("room;{};{};".format(round(mean,2), round(std_deviation,2)))
mean = np.average(keys)
std_deviation = np.std(keys)
file.write("keys;{};{};".format(round(mean,2), round(std_deviation,2)))
mean = np.average(barriers)
std_deviation = np.std(barriers)
file.write("barriers;{};{};".format(round(mean,2), round(std_deviation,2)))
mean = np.average(lcoef)
std_deviation = np.std(lcoef)
file.write("lcoef;{};{};".format(round(mean,2), round(std_deviation,2)))
mean = np.average(nLocks)
std_deviation = np.std(nLocks)
file.write("nLocks;{};{};".format(round(mean,2), round(std_deviation,2)))
mean = np.average(lastFitness)
std_deviation = np.std(lastFitness)
file.write("lastFitness;{};{};".format(round(mean,2), round(std_deviation,2)))
mean = np.average(runTime)
std_deviation = np.std(runTime)
file.write("runTime;{};{};\n".format(round(mean,2), round(std_deviation,2)))

file.close()

# Finally save all the raw data !
rawFile = open(currentFolder + "/RawResults.txt","a")
rawFile.write("{}_{}_{}_{}_{}\n".format(numVertices, numKeys, numLocks, lCoefficient, neededLocks))
# Save the convergence
convStringMin = ""
convStringMax = ""
convStringAvg = ""
for id in range(totalSize):
    convStringMin += str(minF[id]) + ";"
    convStringMax += str(maxF[id]) + ";"
    convStringAvg += str(avgF[id]) + ";"
convStringMin += "\n"
convStringMax += "\n"
convStringAvg += "\n"

rawFile.write(convStringMin)
rawFile.write(convStringMax)
rawFile.write(convStringAvg)

# Now save the heatMatrixData (only the "active")
for y in range(0, currentSize):
    line = ""
    for x in range(0, currentSize): 
      line += str(heatMatrix[x][y]) + ";"
    line += "\n"
    rawFile.write(line)

rawFile.close()

# Finally for the wilcoxon test, we put all the fitness values !
fitnessFile = open(currentFolder + "/RawFitness.txt","a")
fitnessFile.write("{}_{}_{}_{}_{};".format(numVertices, numKeys, numLocks, lCoefficient, neededLocks))
line = ""
for i in range(0, N):
    line += str(lastFitness[i]) + ";"
line += "\n"
fitnessFile.write(line)
fitnessFile.close()
