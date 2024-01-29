import numpy as np
from scipy import stats
analisis = "less" # alternative="less","greater"
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
########################                Wilcoxon Test             #############################
###############################################################################################
# Open 2 folders and make the wilcoxon analisis Analisis_GANOSE and Analisis_GAOR
# Open the files and acquire the resultant fitness
folder = sys.argv[1]
gafile = open(folder+"/Analisis_GASE/RawFitness.txt")
originalfile = open(folder+"/Analisis_GAOR_NODFS/RawFitness.txt")

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

wilcoxonfile = open(folder+"/Wilcoxon2.txt","w")

lines = []
lines.append(["Problem", "Wilcoxon p value"])
counter = 1
for x in finalDataGA:
    # inverted counter for pos and neg, to display the difference in the document
    lines.append([])
    pos = 0
    neg = 0
    tie = 0
    finalDataGA[x].sort()
    finalDataOr[x].sort()

    for p in range(len(finalDataGA[x])):    	
        res = finalDataGA[x][p] - finalDataOr[x][p]
        if res == 0:
            tie+= 1
        elif res < 0:
            pos += 1
        else:
            neg +=1

    total = tie + pos + neg
    resultS = ""
    if(tie != total):
        result = stats.wilcoxon(finalDataGA[x], finalDataOr[x], alternative=analisis)
        formatted_num = f"{result.pvalue:.4e}"  # Using f-string
        resultS = "" + formatted_num
        # Add bf if the result is significant
        if (result.pvalue <= 0.05):
            resultS = "\\textbf{ "+ resultS + " }"
    else:
        resultS = "TIE"
    x = x.replace("_",",")
    x = "(" + x + ")"
    lines[counter].append(x)
    lines[counter].append(resultS)
    counter += 1
wilcoxonfile.write(GenerateTable(lines,2))
wilcoxonfile.close()

gafile.close()
originalfile.close()

