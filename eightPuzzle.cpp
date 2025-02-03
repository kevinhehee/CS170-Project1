#include <bits/stdc++.h>
#include <iostream>
#include <unordered_set>
#include <sstream>

using namespace std;

vector<int> directions{-1, 0, 1, 0, -1};

unordered_map<string, string> uniformCostSearch(vector<vector<int>> puzzle);
unordered_map<string, string> misplacedTileHeuristic(vector<vector<int>> puzzle);
unordered_map<string, string> manhattanDistanceHeuristic(vector<vector<int>> puzzle);

// General Helper Functions
bool inRange(int row, int col, int size);
pair<int,int> findEmptyCoord(vector<vector<int>> &puzzle);
void printPuzzle(const vector<vector<int>> &puzzle);
string stateToString(const vector<vector<int>> &puzzle);
bool isSolutionState(vector<vector<int>> &state);
vector<vector<vector<int>>> traceBackSolutionSteps(unordered_map<string, string> &map, int mapSize);
void printSolution(const vector<vector<vector<int>>> &steps, const vector<vector<int>> &puzzle);

int countMisplacedTiles(vector<vector<int>> &puzzle);

int calculateManhattanDistance(const vector<vector<int>> &puzzle);
int manhattanDistanceCalculator(const pair<int,int> coordA, const pair<int,int> coordB);
pair<int,int> findCurrentRowAndCol(const vector<vector<int>> &puzzle, int number);

string stateToString(const vector<vector<int>> &puzzle) {
    string result;

    for (int r = 0; r < puzzle.size(); r++) {
        for (int c = 0; c < puzzle.at(0).size(); c++) {

            // cout << puzzle.at(r).at(c) << endl;
            result += to_string(puzzle.at(r).at(c)) + ",";
        }
    }
    return result;
}

vector<vector<int>> stringToState(const string &puzzleString, int mapLength) {

    stringstream ss(puzzleString);

    vector<vector<int>> puzzle(mapLength, vector<int>(mapLength, 0));

    int num;

    for (int i = 0; i < mapLength; i++) {
        for (int j = 0; j < mapLength; j++) {
            ss >> num;
            ss.get();
            puzzle.at(i).at(j) = num;

        }
    }

    return puzzle;
}

vector<vector<int>> getSolutionState(int mapLength) {
    int count = 1;
    vector<vector<int>> solution(mapLength, vector<int>(mapLength, 0));

    for (int i = 0; i < mapLength; i++) {
        for (int j = 0; j < mapLength; j++) {

            if (i == mapLength - 1 && j == mapLength - 1) {
                return solution;
            }
            solution.at(i).at(j) = count;
            count++;
        }
    }
    return {{}};
}

vector<vector<vector<int>>> traceBackSolutionSteps(unordered_map<string, string> &map, int mapLength) {

    vector<vector<int>> solutionState = getSolutionState(mapLength);
    string solutionStateString = stateToString(solutionState);

    if (!map.count(solutionStateString)) {
        return {{{}}};
    }

    vector<vector<vector<int>>> solutionSteps{solutionState};

    string currStateString = solutionStateString;

    while (map.at(currStateString) != "") {
        string prevStateString = map.at(currStateString);
        solutionSteps.push_back(stringToState(prevStateString, mapLength));
        currStateString = prevStateString;
    }

    return solutionSteps;
}

unordered_map<string, string> uniformCostSearch(vector<vector<int>> puzzle) {
    queue<vector<vector<int>>> nodes;
    unordered_map<string, string> visitedMap;
    visitedMap.insert({stateToString(puzzle), ""});

    nodes.push(puzzle);

    while (!nodes.empty()) {
        vector<vector<int>> currNode = nodes.front();
        nodes.pop();

        string currStateString = stateToString(currNode);
    
        if (isSolutionState(currNode)) {
            return visitedMap;
        }

        pair<int,int> zeroRowAndCol = findEmptyCoord(currNode);
        int zeroRow = zeroRowAndCol.first;
        int zeroCol = zeroRowAndCol.second;

        for (int dirCount = 0; dirCount < 4; dirCount++) {
            int newRow = zeroRow + directions.at(dirCount);
            int newCol = zeroCol + directions.at(dirCount + 1);

            if (inRange(newRow, newCol, currNode.size())) {
                swap(currNode.at(zeroRow).at(zeroCol), currNode.at(newRow).at(newCol));

                string newStateString = stateToString(currNode);
                if (visitedMap.find(newStateString) == visitedMap.end()) {
                    nodes.push(currNode);
                    visitedMap.insert({newStateString, currStateString});
                }
                swap(currNode.at(zeroRow).at(zeroCol), currNode.at(newRow).at(newCol));
            }
        }
    }

    return visitedMap;
}

unordered_map<string, string> misplacedTileHeuristic(vector<vector<int>> puzzle) {
    priority_queue<pair<int, vector<vector<int>>>, vector<pair<int, vector<vector<int>>>>, greater<pair<int, vector<vector<int>>>>> nodes;
    unordered_map<string, string> visitedMap;

    visitedMap.insert({stateToString(puzzle), ""});
    nodes.push({countMisplacedTiles(puzzle), puzzle});

    while (!nodes.empty()) {
        vector<vector<int>> currNode = nodes.top().second;
        nodes.pop();

        string currStateString = stateToString(currNode);

        if (isSolutionState(currNode)) {
            return visitedMap;
        }

        pair<int,int> zeroRowAndCol = findEmptyCoord(currNode);
        int zeroRow = zeroRowAndCol.first;
        int zeroCol = zeroRowAndCol.second;

        for (int dirCount = 0; dirCount < 4; dirCount++) {
            int newRow = zeroRow + directions.at(dirCount);
            int newCol = zeroCol + directions.at(dirCount + 1);

            if (inRange(newRow, newCol, currNode.size())) {
                swap(currNode.at(zeroRow).at(zeroCol), currNode.at(newRow).at(newCol));

                string newStateString = stateToString(currNode);
                if (visitedMap.find(newStateString) == visitedMap.end()) {
                    nodes.push({countMisplacedTiles(currNode), currNode});
                    visitedMap.insert({newStateString, currStateString});
                }
                swap(currNode.at(zeroRow).at(zeroCol), currNode.at(newRow).at(newCol));
            }
        }
    }

    return visitedMap;
}


unordered_map<string, string> manhattanDistanceHeuristic(vector<vector<int>> puzzle) {

    unordered_map<string, string> visitedMap;
    priority_queue<pair<int, vector<vector<int>>>, vector<pair<int, vector<vector<int>>>>, greater<pair<int, vector<vector<int>>>>> nodes;

    visitedMap.insert({stateToString(puzzle), ""});
    nodes.push({calculateManhattanDistance(puzzle), puzzle});

    while (!nodes.empty()) {

        vector<vector<int>> currNode = nodes.top().second;
        nodes.pop();

        string currStateString = stateToString(currNode);

        if (isSolutionState(currNode)) {
            return visitedMap;
        }

        pair<int,int> zeroRowAndCol = findEmptyCoord(currNode);
        int zeroRow = zeroRowAndCol.first;
        int zeroCol = zeroRowAndCol.second;

        for (int dirCount = 0; dirCount < 4; dirCount++) {
            int newRow = zeroRow + directions.at(dirCount);
            int newCol = zeroCol + directions.at(dirCount + 1);

            if (inRange(newRow, newCol, currNode.size())) {
                swap(currNode.at(zeroRow).at(zeroCol), currNode.at(newRow).at(newCol));

                string newStateString = stateToString(currNode);
                if (visitedMap.find(newStateString) == visitedMap.end()) {
                    nodes.push({calculateManhattanDistance(currNode), currNode});
                    visitedMap.insert({newStateString, currStateString});
                }
                swap(currNode.at(zeroRow).at(zeroCol), currNode.at(newRow).at(newCol));
            }
        }
    }

    return visitedMap;
}


int calculateManhattanDistance(const vector<vector<int>> &puzzle) {

    int currNumber = 1;
    int totalManhattanDistance = 0;
    int maxNumber = puzzle.size() * puzzle.size();

    for (int row = 0; row < puzzle.size(); row++) {
        for (int col = 0; col < puzzle.at(0).size(); col++) {

            if (puzzle.at(row).at(col) == currNumber) {
                currNumber++;
                continue;
            }

            if (currNumber == maxNumber) {
                return totalManhattanDistance;
            }

            pair<int, int> correctRowAndCol = {row, col};
            pair<int, int> currentRowAndCol = findCurrentRowAndCol(puzzle, currNumber);
            totalManhattanDistance += manhattanDistanceCalculator(correctRowAndCol, currentRowAndCol);

            // cout << endl;
            // cout << "correct row and col of: " << currNumber << ": (" << correctRowAndCol.first << ", " << correctRowAndCol.second << ")" << endl;
            // cout << "current row and col of: " << currNumber << ": (" << currentRowAndCol.first << ", " << currentRowAndCol.second << ")" << endl;

            // cout << "Added " << manhattanDistanceCalculator(correctRowAndCol, currentRowAndCol) << endl;
            // printPuzzle(puzzle);
            // cout << endl;
            
            currNumber++;
        }
    }

    // cout << "Manhattan Distance of puzzle below is: " << totalManhattanDistance << endl;
    // printPuzzle(puzzle);
    // cout << "-----------------------------------------" << endl;

    return totalManhattanDistance;

}

int manhattanDistanceCalculator(const pair<int,int> coordA, const pair<int,int> coordB) {

    return (abs(coordA.first - coordB.first) + abs(coordA.second - coordB.second));
}

pair<int,int> findCurrentRowAndCol(const vector<vector<int>> &puzzle, int number) {
    int sideLength = puzzle.size();

    for (int r = 0; r < puzzle.size(); r++) {
        for (int c = 0; c < puzzle.at(0).size(); c++) {
            if (puzzle.at(r).at(c) == number) {
                return {r, c};
            }
        }
    }
    return {-1, -1};
}

bool inRange(int row, int col, int size) {
    if (row >= 0 && col >= 0 && row < size && col < size) {
        return true;
    }
    return false;
}

pair<int,int> findEmptyCoord(vector<vector<int>> &puzzle) {
    for (int r = 0; r < puzzle.size(); r++) {
        for (int c = 0; c < puzzle.at(0).size(); c++) {
            if (puzzle.at(r).at(c) == 0) {
                return {r, c};
            }
        }
    }
    return {-1, -1};
}

int countMisplacedTiles(vector<vector<int>> &puzzle) {
    int currNumber = 1;
    int misplacedTiles = 0;
    int maxNumber = puzzle.size() * puzzle.size();

    for (int row = 0; row < puzzle.size(); row++) {
        for (int col = 0; col < puzzle.at(0).size(); col++) {

        

            if (puzzle.at(row).at(col) != currNumber) {
                misplacedTiles++;
            }



            currNumber++;
        }
    }

    return 0;
}

void printPuzzle(const vector<vector<int>> &puzzle) {
    for (int r = 0; r < puzzle.size(); r++) {
        cout << "\t";
        for (int c = 0; c < puzzle.at(0).size(); c++) {
            cout << puzzle.at(r).at(c);
        }
        cout << endl;
    }
    cout << endl;
}

bool isSolutionState(vector<vector<int>> &puzzle) {
    int currNumber = 1;

    int maxNumber = puzzle.size() * puzzle.size();

    for (int row = 0; row < puzzle.size(); row++) {
        for (int col = 0; col < puzzle.at(0).size(); col++) {

            if (currNumber == maxNumber) {
                break;
            }

            if (puzzle.at(row).at(col) != currNumber) {
                return false;
            }

            currNumber++;
        }
    }

    return true;
}

void printTime(chrono::duration<double> elapsedSeconds, int searchType) {

    double timeSeconds = elapsedSeconds.count();
    double timeMs = timeSeconds * 1000;

    if (searchType == 1) {
        cout << "Uniform Cost Search: " << endl;
        cout << "Time to search: " << timeMs << " ms" << endl;

    } else if (searchType == 2) {
        cout << "Misplaced Tile Heuristic A-Star Search: " << endl;
        cout << "Time to search: " << timeMs << " ms" << endl;
        
    } else if (searchType == 3) {
        cout << "Manhattan Distance Heuristic A-Star Search: " << endl;
        cout << "Time to search: " << timeMs << " ms" << endl;
    }

}

void printSolution(const vector<vector<vector<int>>> &solutionSteps, const vector<vector<int>> &puzzle) {

    int stepCount = 1;

    cout << "Solution Path Length: " << solutionSteps.size() << endl;
    cout << endl;
    cout << "\tStarting State" << endl;
    printPuzzle(puzzle);

    for (int i = 1; i < (int)solutionSteps.size() - 1; i++) {
        cout << "\tStep: " << stepCount++ << endl;
        printPuzzle(solutionSteps.at(i));
    }
    cout << "\tSolution Step (" << stepCount << ")" << endl;
    printPuzzle(solutionSteps.back());
    
}



int main() {
    int testCases = 0;

    cout << "Enter testcase count" << endl;
    cin >> testCases;

    int puzzleLength = 0;
    cin >> puzzleLength;

    int testCase = 1;

    cout << endl;
    while (testCases--) {

        cout << "TEST CASE: " << testCase++ << endl;
        cout << "-------------------------------------" << endl;

        vector<vector<int>> puzzle(puzzleLength, vector<int>());

        string row;

        for (int i = 0; i < puzzleLength; i++) {
            cin >> row;

            stringstream ss(row);
            int val;

            for (int j = 0; j < row.size(); j++) {
                // puzzle.at(i).push_back(row.at(j) - '0');
                ss >> val;
                ss.get();
                puzzle.at(i).push_back(val);
            }
        }

        {
            // Uniform Cost Search
            auto startTime = chrono::high_resolution_clock::now();
            unordered_map<string, string> puzzleMap = uniformCostSearch(puzzle);
            auto endTime = chrono::high_resolution_clock::now();

            printTime(endTime - startTime, 1);

            string solutionStateString = stateToString(getSolutionState(puzzleLength));

            if (puzzleMap.count(solutionStateString)) {
                vector<vector<vector<int>>> solutionSteps = traceBackSolutionSteps(puzzleMap, puzzleLength);

                reverse(solutionSteps.begin(), solutionSteps.end());

                printSolution(solutionSteps, puzzle);

            } else {
                cout << "No Solution" << endl;
            }
        }

        cout << endl;

        {
            // Misplaced Tile Heuristic A-Star Search
            auto startTime = chrono::high_resolution_clock::now();
            unordered_map<string, string> puzzleMap = misplacedTileHeuristic(puzzle);
            auto endTime = chrono::high_resolution_clock::now();

            printTime(endTime - startTime, 2);
            string solutionStateString = stateToString(getSolutionState(puzzleLength));

            if (puzzleMap.count(solutionStateString)) {
                vector<vector<vector<int>>> solutionSteps = traceBackSolutionSteps(puzzleMap, puzzleLength);
                reverse(solutionSteps.begin(), solutionSteps.end());
                printSolution(solutionSteps, puzzle);

            } else {
                cout << "No Solution" << endl;
            }
        }

        cout << endl;

        {
            // Manhattan Distance Heuristic A-Star Search
            auto startTime = chrono::high_resolution_clock::now();
            unordered_map<string, string> puzzleMap = manhattanDistanceHeuristic(puzzle);
            auto endTime = chrono::high_resolution_clock::now();
            
            printTime(endTime - startTime, 3);

            string solutionStateString = stateToString(getSolutionState(puzzleLength));

            if (puzzleMap.count(solutionStateString)) {
                vector<vector<vector<int>>> solutionSteps = traceBackSolutionSteps(puzzleMap, puzzleLength);
                reverse(solutionSteps.begin(), solutionSteps.end());
                printSolution(solutionSteps, puzzle);

            } else {
                cout << "No Solution" << endl;
            }
        }

        cout << endl;

    }



    return 0;
}

