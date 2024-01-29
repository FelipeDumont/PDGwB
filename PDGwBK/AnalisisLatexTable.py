import sys

# Assume the lines will be lines[Row][Column]
def GenerateTable(lines, col):
    resultLatexTable = ""
    row = len(lines)
    if(row > 0 ):
        resultLatexTable += "\\begin{figure}[h] \n"
        resultLatexTable += "\\begin{center} \n"
        resultLatexTable += "\\begin{tabular}{|" + ("l|"*col) + "} \n"
        resultLatexTable += "\\hline \n"
        for r in range(len(lines)):
            for c in range(len(lines[r])):
                resultLatexTable += str(lines[r][c])
                if c < len(lines[r]) - 1:
                    resultLatexTable += " & "
            resultLatexTable += "\\\\ \\hline  \n"
    
    resultLatexTable += "\\end{tabular} \n"
    resultLatexTable += "\\end{center} \n"
    resultLatexTable += "\\end{figure} \n"
    return resultLatexTable

###############################################################################################
########################                Main Results              #############################
###############################################################################################
import os

'''
mainFolders = ["Results"]
actualMainFolders = []
for folder_path in mainFolders:
    if os.path.exists(folder_path):
        actualMainFolders.append(folder_path)
'''
actualMainFolders = []
actualMainFolders.append(sys.argv[1])
current = actualMainFolders[0]

finalData2Step = {}
finalDataOr = {}


for folder in actualMainFolders:

    gafile = open(folder + "/Analisis_GASE/Results.txt")
    originalfile = open(folder + "/Analisis_GAOR_NODFS/Results.txt")

    finalData2Step[folder] = {}
    finalDataOr[folder] = {}

    for line in gafile:
        line = line.strip().split(";")
        finalData2Step[folder][line[0]] = {}
        for id in range(0,int(len(line[1:-1])/3)):
            base = 1 + id * 3
            finalData2Step[folder][line[0]][line[base]] = [float(line[base + 1]), float(line[base+2])]
    

    for line in originalfile:
        line = line.strip().split(";")
        finalDataOr[folder][line[0]] = {}
        for id in range(0,int(len(line[1:-1])/3)):
            base = 1 + id * 3
            finalDataOr[folder][line[0]][line[base]] = [float(line[base + 1]), float(line[base+2])]

    gafile.close()
    originalfile.close()


# For now just 1 now
currentProblem = {}
analisisfile = open(current + "/Analisis.txt","w")
# Now Make the table !!!
lines = []
lines.append(["", "\multicolumn{4}{c|}{NRooms}", "\multicolumn{4}{c|}{NLocks}", "\multicolumn{4}{c|}{NBarriers}"])
lines.append(["",   "\multicolumn{2}{c|}{2Step}", "\multicolumn{2}{c|}{CEA}", "\multicolumn{2}{c|}{2Step}", "\multicolumn{2}{c|}{CEA}", 
                    "\multicolumn{2}{c|}{2Step}", "\multicolumn{2}{c|}{CEA}"
            ])
lines.append(["Problem",    "Avg.", "st. dv.", "Avg.", "st. dv.", "Avg.", "st. dv.", "Avg.", "st. dv.","Avg.", "st. dv.",
                            "Avg.", "st. dv."])

ignoreList = ["lastFitness", "nLocks", "lcoef", "runTime"]
# lines.append(["", ""])
counter = 3
current = actualMainFolders[0]
for x in finalData2Step[current]:
    problem = x.replace("_",",")
    problem = "(" + problem + ")"
    
    if(problem not in currentProblem):
        data = x.split("_")
        currentProblem[problem] = {}
        currentProblem[problem]["room"] = data[0]
        currentProblem[problem]["keys"] = data[1]
        currentProblem[problem]["barriers"] = data[2]
        currentProblem[problem]["lcoef"] = data[3]
        currentProblem[problem]["runTime"] = 0
        currentProblem[problem]["lastFitness"] = 0
    

    lines.append([])
    
    lines[counter].append(problem)
    for key in finalData2Step[current][x]:
        if key not in ignoreList:
            avg2Step = finalData2Step[current][x][key][0]
            st2Step = finalData2Step[current][x][key][1]
            
            avgOR = finalDataOr[current][x][key][0]
            stOR = finalDataOr[current][x][key][1]
            
            if (abs(float(currentProblem[problem][key]) - float(avg2Step))  < abs(float(currentProblem[problem][key]) - float(avgOR))):
                avg2Step = "\\textbf{ "+ str(avg2Step) + "}"
                avgOR = str(avgOR)
            elif (abs(float(currentProblem[problem][key]) - float(avg2Step))  > abs(float(currentProblem[problem][key]) - float(avgOR))):
                avg2Step = str(avg2Step)
                avgOR = "\\textbf{ "+ str(avgOR) + "}"
            else:
                avg2Step = "\\textbf{ "+ str(avg2Step) + "}"
                avgOR = "\\textbf{ "+ str(avgOR) + "}"

            lines[counter].append(avg2Step)
            lines[counter].append(st2Step)
            lines[counter].append(avgOR)
            lines[counter].append(stOR)
    counter += 1
analisisfile.write(GenerateTable(lines,13))

# Second ...
lines = []
lines.append(["",   "\multicolumn{4}{c|}{L. Coef}", "\multicolumn{4}{c|}{Fitness}", "\multicolumn{4}{c|}{Time (s)}"])
lines.append(["",   "\multicolumn{2}{c|}{2Step}", "\multicolumn{2}{c|}{CEA}", "\multicolumn{2}{c|}{2Step}", "\multicolumn{2}{c|}{CEA}", "\multicolumn{2}{c|}{2Step}", "\multicolumn{2}{c|}{CEA}"
            ])
lines.append(["Problem",    "Avg.", "st. dv.", "Avg.", "st. dv.", "Avg.", "st. dv.", "Avg.", "st. dv.", "Avg.", "st. dv.", "Avg.", "st. dv."])

ignoreList = ["nLocks", "room", "keys", "barriers"]
counter = 3
current = actualMainFolders[0]
for x in finalData2Step[current]:
    problem = x.replace("_",",")
    problem = "(" + problem + ")"
    
    lines.append([])
    
    lines[counter].append(problem)
    for key in finalData2Step[current][x]:
        if key not in ignoreList:
            avg2Step = finalData2Step[current][x][key][0]
            st2Step = finalData2Step[current][x][key][1]
            avgOR = finalDataOr[current][x][key][0]
            stOR = finalDataOr[current][x][key][1]
            
            if (abs(float(currentProblem[problem][key]) - float(avg2Step))  < abs(float(currentProblem[problem][key]) - float(avgOR))):
                avg2Step = "\\textbf{ "+ str(avg2Step) + "}"
                avgOR = str(avgOR)
            elif (abs(float(currentProblem[problem][key]) - float(avg2Step))  > abs(float(currentProblem[problem][key]) - float(avgOR))):
                avg2Step = str(avg2Step)
                avgOR = "\\textbf{ "+ str(avgOR) + "}"
            else:
                avg2Step = "\\textbf{ "+ str(avg2Step) + "}"
                avgOR = "\\textbf{ "+ str(avgOR) + "}"
            
            lines[counter].append(avg2Step)
            lines[counter].append(st2Step)
            lines[counter].append(avgOR)
            lines[counter].append(stOR)
    counter += 1
analisisfile.write(GenerateTable(lines,13))

analisisfile.close()
