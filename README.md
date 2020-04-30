# LFPC Labs
Here are the labs for task 2.1, 2.2 and 3

## Lab 2.1
* Language: `python3`
* Libs: `pprint`
Input file rules:
```
    file contains lines of values separated by space

    line[0] - [Starting state, Final state]
    line[1] - [...states]
    line[2] - [...transitions]
    line[3:n] - transitionRules(startstate, transition, nextState)
```

## Lab 2.2
* Language: `C++`
* Libs: `bimap.h`
* Additional notes: was developed with Visual Studio 2019. Testing the project in this evn should work fine.

For now, supports function definitions, calls and binary operations
```
function test (x)
    test(x + 1);
```

## Lab 3
* Language: `python3`
```
    file contains lines of values separated by space

    line[0] - [...nonterminalSymbols]
    line[1] - [...terminalSymbols]
    line[2:b] - production(startState, productionRHS)
```
note: ε is denoted by `-`