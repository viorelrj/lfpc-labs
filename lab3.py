import random
import string

#CNF
eps = '-'

data = open('lab3.txt').read().split('\n')
nonterminals = data[0].split(' ')
terminals = data[1].split(' ')
del data[0:2]

productions = list(map(lambda item: item.split(' '), data))

def eliminateNullStateProductions(productions):
    def getAbsencePowerSet(state, string):
        states = []
        indexes = []
        for index, item in enumerate(string):
            if (item == state):
                indexes.append(index)
        
        for binaryRepresentation in range(1, 2**len(indexes)):
            template = '{0:0%db}' % len(indexes)
            binaruNumber = template.format(binaryRepresentation)
            orders = [indexes[index] * int(digit) for index, digit in enumerate(binaruNumber) if digit != '0']
            state = [char for index, char in enumerate(string) if index not in orders]
            states.append(''.join(state))
        return states

    nullProductions = []
    bannedIndexes = []

    for index, production in enumerate(productions):
        if (production[1]) == eps:
            bannedIndexes.append(index)
            nullProductions.append(production[0])
    filteredProductions = [elem for index, elem in enumerate(productions) if index not in bannedIndexes]
    
    
    biStateProductions = filteredProductions[:]
    for nullState in nullProductions:
        for production in filteredProductions:
            if nullState in production[1]:
                biStateProductions += list(map(lambda x: [production[0], x], getAbsencePowerSet(nullState, production[1])))

    returnList = [item for item in biStateProductions if item[1] != '']
    return returnList

def eliminateRenamings(productions):
    iterateableProductions = productions[:]
    removableIndexes = []
    stash = []

    def getTerminal(index):
        source = iterateableProductions[index][0]
        target = iterateableProductions[index][1]
        res = []
        for i, item in enumerate(iterateableProductions):
            if item[0] == target:
                redirect = item[1]
                if len(redirect) == 1 and redirect in nonterminals:
                    res += getTerminal(i)
                else:
                    res.append([source, item[1]])
        return res

    for index, item in enumerate(iterateableProductions):
        if len(item[1]) == 1 and item[1] in nonterminals:
            removableIndexes.append(index)
            stash += getTerminal(index)
    
    productions = [item for index, item in enumerate(productions) if index not in removableIndexes]

    return list(productions) # eliminate repetitions

def eliminateUnproductive(productions):
    productive = terminals[:]

    while True:
        initialLength = len(productive)
        for index, item in enumerate(productions):
            isProductive = True
            for char in item[1]:
                if char not in productive:
                    isProductive = False
                    break
            if isProductive and item[0] not in productive:
                productive.append(item[0])
        if initialLength == len(productive):
            break
    
    def filterFunction(item):
        if item[0] in nonproductive:
            return False
        for char in item[1]:
            if char in nonproductive:
                return False
        return True

    nonproductive = [item for item in list(map(lambda x: x[0], productions)) if item not in productive]
    return list(filter(filterFunction, productions))

def eliminateInaccessible(productions):
    inaccessible = nonterminals[:]

    for item in productions:
        for char in item[1]:
            if char in inaccessible:
                inaccessible.remove(char)
    
    return list(filter(lambda x: x[0] not in inaccessible, productions))

def normalizeCNF(productions):
    normalized = []
    newNonterminals = []

    def getNewNonterminal():
        candidate = nonterminals[0]
        while candidate in nonterminals:
            candidate = random.choice(string.ascii_letters.capitalize())
        return candidate.capitalize()

    def nonterminalize(string):
        word = []
        for char in string:
            if char in terminals:
                for item in normalized:
                    if item[1] == char:
                        word.append(item[0])
                        break
            else:
                word.append(char)
        return ''.join(word)


    def filterFunction(item):
        if len(item[1]) > 2:
            return True
        elif len(item[1]) == 2:
            for char in item[1]:
                if char in terminals:
                    return True

    invalid = list(filter(filterFunction, productions))
    normalized = list(filter(lambda x: x not in invalid, productions))

    neighbourTerminals = []
    for item in productions:
        for char in item[1]:
            if char in terminals and char not in neighbourTerminals:
                neighbourTerminals.append(char)

    for item in neighbourTerminals:
        productionIsIncluded = False
        for production in productions:
            if production[1] == item:
                productionIsIncluded=  True
                break
        if not productionIsIncluded:
            normalized.append([getNewNonterminal(), item])

    invalid = list(map(lambda x: [x[0], nonterminalize(x[1])], invalid))
    normalized += list(filter(lambda x: len(x[1]) == 2, invalid))
    invalid = list(filter(lambda x: x not in normalized, invalid))

    while len(invalid) > 0:
        for item in invalid:
            foundMatch = False
            for normal in normalized:
                i = list(item[1])
                if normal[1] in item[1]:
                    index = item[1].find(normal[1])
                    del i[index]
                    del i[index]
                    i.insert(index, normal[0])
                    item[1] = ''.join(i)
                    foundMatch = True
                    break
            if not foundMatch:
                newStartState = getNewNonterminal()
                pair = item[1][-2:]
                item[1] = item[1][0:-2] + newStartState
                normalized.append([newStartState, pair])
        normalized += list(filter(lambda x: len(x[1]) == 2, invalid))
        invalid = list(filter(lambda x: x not in normalized, invalid))

    return normalized


productions = eliminateNullStateProductions(productions)
productions = eliminateRenamings(productions)
productions = eliminateUnproductive(productions)
productions = eliminateInaccessible(productions)
productions = normalizeCNF(productions)

print(productions)