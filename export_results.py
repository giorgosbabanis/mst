import sys
import re

inputFileName = sys.argv[1]
inputFile = open(inputFileName, 'r')
outputFileName = inputFileName.removesuffix(".out") + ".csv"
outputFile = open(outputFileName, 'w+')

preTime = 0
iterations = 0

outputFile.write("name, Exec Time, Iterations, Prep Time, Weight, Edges\n")

for line in inputFile:
    if "Running for graph:" in line:
        name = line.replace("Running for graph:", "").strip().removeprefix("graphs/")
        continue
    if "[3;34mIt" in line:
        #time = re.findall(r"[-+]?(?:\d*\.*\d+)", line)[5]
        iterations+=1
        continue
    if "[3;34mPre" in line:
        numbers = re.findall(r"[-+]?(?:\d*\.*\d+)", line)
        time = numbers[len(numbers) - 1]
        preTime += float(time)
        continue
    if "finished mst" in line:
        outputFile.write(name + "," + execTime + "," + str(iterations) + "," + str(round(preTime,2)) + "," + weight + "," + edges + ",\n")
        preTime = 0
        iterations = 0
        name = "2nd run"
    if "Exec. time" in line:
        execTime = re.findall(r"[-+]?(?:\d*\.*\d+)", line)[0]
    if "Forest weight:" in line:
        weight = re.findall(r"[-+]?(?:\d*\.*\d+)", line)[2]
        edges = re.findall(r"[-+]?(?:\d*\.*\d+)", line)[7]
    
inputFile.close()
outputFile.close()

 