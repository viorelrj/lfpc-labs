import pprint

data = open('lab2.1.txt').read()
lines = data.split('\n')

[start, end] = lines[0].split(' ')
del lines[0:1]

states = lines[0].split(' ')
transitions = lines[1].split(' ')
del lines[0:2]

transitionTable = {}
for stateKey in states:
    stateDict = {}
    for transition in transitions:
        stateDict[transition] = []
    transitionTable[stateKey] = stateDict


for line in lines:
    [x, y, val]  = line.split(' ')
    transitionTable[x][y].append(val)


def getState(arr):
    # states = arr.split(' ')
    if (len(arr) == 0):
        return ''
    states = arr
    states.sort()
    return ' '.join(states)

tempTable = {}


flag = False
while not flag:
    flag = False
    for stateKey in transitionTable:
        for transitionKey in transitionTable[stateKey]:
            newState = getState(transitionTable[stateKey][transitionKey])
            if not newState in transitionTable.keys() and len(newState) > 0:
                flag = True
                newTransition = {}
                for trans in transitions:
                    arr = []
                    for key in newState.split(' '):
                        arr += transitionTable[key][trans]
                    newTransition[trans] = arr
                tempTable[newState] = newTransition
    
    for key in tempTable:
        transitionTable[key] = tempTable[key]
    
    tempTable = {}

for x in transitionTable:
    for y in transitionTable[x]:
        transitionTable[x][y] = getState(transitionTable[x][y])

pp = pprint.PrettyPrinter()
pp.pprint(transitionTable)
