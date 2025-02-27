#include <bits/stdc++.h>
#include <iostream>
#include <unordered_set>
#include <sstream>
#include <iomanip>

using namespace std;



vector<int> directions{-1, 0, 1, 0, -1};

int countMisplacedTiles(const vector<vector<int>> &puzzle);

int calculateManhattanDistance(const vector<vector<int>> &puzzle);

struct State {
    
    vector<vector<int>> puzzle;
    int stepsTaken;
    int manhattanDistance;
    int misplacedTiles;
    int queueSize;
    std::__1::chrono::steady_clock::time_point time;

    State() {
        this->puzzle = {};
        this->stepsTaken = -1;
        this->manhattanDistance = -1;
        this->misplacedTiles = -1;
        this->queueSize = 1;
    }

    State(const vector<vector<int>> &puzzle, int stepsTaken, int queueSize) {
        this->puzzle = vector<vector<int>>(puzzle);
        this->stepsTaken = stepsTaken;
        this->manhattanDistance = calculateManhattanDistance(puzzle);
        this->misplacedTiles = countMisplacedTiles(puzzle);
        this->queueSize = queueSize;
        this->time = chrono::high_resolution_clock::now();
    }

};
// General Helper Functions
bool inRange(int row, int col, int size);
pair<int,int> findEmptyCoord(vector<vector<int>> &puzzle);
void printPuzzle(const vector<vector<int>> &puzzle);
string stateToString(const vector<vector<int>> &puzzle);
bool isSolutionState(vector<vector<int>> &state);
// vector<State> traceBackSolutionSteps(unordered_map<string, string> &map, int mapSize);
void printSolution(const vector<State> &, const vector<vector<int>> &puzzle);

int manhattanDistanceCalculator(const pair<int,int> coordA, const pair<int,int> coordB);
pair<int,int> findCurrentRowAndCol(const vector<vector<int>> &puzzle, int number);

int ucsMaxQueueSize = 0;
int misplacedMaxQueueSize = 0;
int manhattanMaxQueueSize = 0;


struct misplacedTileComparator {
    bool operator() (const State &a, const State &b) const {
        return a.stepsTaken + a.misplacedTiles > b.stepsTaken + b.misplacedTiles;
    }
};

struct manhattanDistanceComparator {
    bool operator() (const State &a, const State &b) const {
        return a.stepsTaken + a.manhattanDistance > b.stepsTaken + b.manhattanDistance;
    }
};
string stateToString(const vector<vector<int>> &puzzle) {
    string result;

    for (int r = 0; r < puzzle.size(); r++) {
        for (int c = 0; c < puzzle.at(0).size(); c++) {
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

vector<State> traceBackSolutionSteps(unordered_map<string, State> &map, int mapLength) {

    vector<vector<int>> solutionState = getSolutionState(mapLength);
    string solutionStateString = stateToString(solutionState);

    if (!map.count(solutionStateString)) {
        return {{{}}};
    }

    // vector<vector<vector<int>>> solutionSteps{solutionState};

    vector<State> solutionSteps{State(solutionState, -1, -1)};

    string currStateString = solutionStateString;

    while (map.at(currStateString).puzzle.size() != 0) {
        State prevState = map.at(currStateString);
        solutionSteps.push_back(prevState);
        currStateString = stateToString(prevState.puzzle);
    }

    return solutionSteps;
}

unordered_map<string, State> uniformCostSearch(vector<vector<int>> puzzle) {
    queue<State> nodes;
    unordered_map<string, State> visitedMap;
    visitedMap.insert({stateToString(puzzle), State()});
    nodes.push(State(puzzle, 0, 1));

    int maxQueueSize = 1;

    while (!nodes.empty()) {

        maxQueueSize = max(maxQueueSize, (int)nodes.size());
        State currNode = nodes.front();
        int gN = currNode.stepsTaken;
        nodes.pop();

        string currStateString = stateToString(currNode.puzzle);
    
        if (isSolutionState(currNode.puzzle)) {
                        cout << "SOLUTION END QUEUE SIZE: "<< nodes.size() << endl;

            ucsMaxQueueSize = maxQueueSize;
            return visitedMap;
        }

        pair<int,int> zeroRowAndCol = findEmptyCoord(currNode.puzzle);
        int zeroRow = zeroRowAndCol.first;
        int zeroCol = zeroRowAndCol.second;

        for (int dirCount = 0; dirCount < 4; dirCount++) {
            int newRow = zeroRow + directions.at(dirCount);
            int newCol = zeroCol + directions.at(dirCount + 1);

            if (inRange(newRow, newCol, currNode.puzzle.size())) {
                swap(currNode.puzzle.at(zeroRow).at(zeroCol), currNode.puzzle.at(newRow).at(newCol));
                vector<vector<int>> newState = vector<vector<int>>(currNode.puzzle);
                swap(currNode.puzzle.at(zeroRow).at(zeroCol), currNode.puzzle.at(newRow).at(newCol));

                string newStateString = stateToString(newState);
                if (visitedMap.find(newStateString) == visitedMap.end()) {
                    State newNode = State(newState, gN + 1, nodes.size());
                    nodes.push(newNode);
                    visitedMap.insert({newStateString, currNode});
                }
            }
        }
    }

    return visitedMap;
}

unordered_map<string, State> misplacedTileHeuristic(vector<vector<int>> puzzle) {
    priority_queue<State, vector<State>, misplacedTileComparator> nodes; 
    unordered_map<string, State> visitedMap;
    
    visitedMap.insert({stateToString(puzzle), State()});
    nodes.push(State(puzzle, 0, 1));
    int maxQueueSize = 1;

    while (!nodes.empty()) {
        maxQueueSize = max(maxQueueSize, (int)nodes.size());
        State currNode = nodes.top();
        int gN = currNode.stepsTaken;
        nodes.pop();

        string currStateString = stateToString(currNode.puzzle);

        if (isSolutionState(currNode.puzzle)) {
            cout << "SOLUTION END QUEUE SIZE: "<< nodes.size() << endl;
            misplacedMaxQueueSize = maxQueueSize;
            return visitedMap;
        }

        pair<int,int> zeroRowAndCol = findEmptyCoord(currNode.puzzle);
        int zeroRow = zeroRowAndCol.first;
        int zeroCol = zeroRowAndCol.second;

        for (int dirCount = 0; dirCount < 4; dirCount++) {
            int newRow = zeroRow + directions.at(dirCount);
            int newCol = zeroCol + directions.at(dirCount + 1);

            if (inRange(newRow, newCol, currNode.puzzle.size())) {
                swap(currNode.puzzle.at(zeroRow).at(zeroCol), currNode.puzzle.at(newRow).at(newCol));
                vector<vector<int>> newState = vector<vector<int>>(currNode.puzzle);
                swap(currNode.puzzle.at(zeroRow).at(zeroCol), currNode.puzzle.at(newRow).at(newCol));

                string newStateString = stateToString(newState);
                if (visitedMap.find(newStateString) == visitedMap.end()) {
                    State newNode = State(newState, gN + 1, nodes.size());
                    nodes.push(newNode);
                    visitedMap.insert({newStateString, currNode});
                }
            }
        }
    }

    return visitedMap;
}


unordered_map<string, State> manhattanDistanceHeuristic(vector<vector<int>> puzzle) {
    priority_queue<State, vector<State>, manhattanDistanceComparator> nodes; 
    unordered_map<string, State> visitedMap;

    visitedMap.insert({stateToString(puzzle), State()});
    nodes.push(State(puzzle, 0, 1));
    int maxQueueSize = 1;

    while (!nodes.empty()) {
        maxQueueSize = max(maxQueueSize, (int)nodes.size());
        State currNode = nodes.top();
        int gN = currNode.stepsTaken;
        nodes.pop();

        string currStateString = stateToString(currNode.puzzle);

        if (isSolutionState(currNode.puzzle)) {
                        cout << "SOLUTION END QUEUE SIZE: "<< nodes.size() << endl;

            manhattanMaxQueueSize = maxQueueSize;
            return visitedMap;
        }

        pair<int,int> zeroRowAndCol = findEmptyCoord(currNode.puzzle);
        int zeroRow = zeroRowAndCol.first;
        int zeroCol = zeroRowAndCol.second;

        for (int dirCount = 0; dirCount < 4; dirCount++) {
            int newRow = zeroRow + directions.at(dirCount);
            int newCol = zeroCol + directions.at(dirCount + 1);

            if (inRange(newRow, newCol, currNode.puzzle.size())) {
                swap(currNode.puzzle.at(zeroRow).at(zeroCol), currNode.puzzle.at(newRow).at(newCol));
                vector<vector<int>> newState = vector<vector<int>>(currNode.puzzle);
                swap(currNode.puzzle.at(zeroRow).at(zeroCol), currNode.puzzle.at(newRow).at(newCol));

                string newStateString = stateToString(newState);
                if (visitedMap.find(newStateString) == visitedMap.end()) {
                    State newNode = State(newState, gN + 1, nodes.size());
                    nodes.push(newNode);
                    visitedMap.insert({newStateString, currNode});
                }
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
            
            currNumber++;
        }
    }
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

int countMisplacedTiles(const vector<vector<int>> &puzzle) {
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

    return misplacedTiles;
}

void printPuzzle(const vector<vector<int>> &puzzle) {
    for (int r = 0; r < puzzle.size(); r++) {
        cout << "\t";
        for (int c = 0; c < puzzle.at(0).size(); c++) {
            cout << puzzle.at(r).at(c) << " ";
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
        cout << "Max queue size: " << ucsMaxQueueSize << " nodes" << endl;

    } else if (searchType == 2) {
        cout << "Misplaced Tile Heuristic A-Star Search: " << endl;
        cout << "Time to search: " << timeMs << " ms" << endl;
        cout << "Max queue size: " << misplacedMaxQueueSize << " nodes" << endl;
        
    } else if (searchType == 3) {
        cout << "Manhattan Distance Heuristic A-Star Search: " << endl;
        cout << "Time to search: " << timeMs << " ms" << endl;
        cout << "Max queue size: " << manhattanMaxQueueSize << " nodes" << endl;
    }

}

void printSolution(const vector<State> &solutionSteps, const vector<vector<int>> &puzzle) {

    int stepCount = 1;
    cout << endl;
    // cout << "\tStarting State" << endl;
    // printPuzzle(puzzle);

    // for (int i = 1; i < (int)solutionSteps.size() - 1; i++) {
    //     cout << "\tStep: " << stepCount++ << endl;
    //     printPuzzle(solutionSteps.at(i).puzzle);
    // }
    // cout << "\tSolution Step (" << stepCount << ")" << endl;
    // printPuzzle(solutionSteps.back().puzzle);

    // cout << "solution queue size at each step" << endl;
    // for (int i = 1; i < (int)solutionSteps.size() - 1; i++) {
    //     // cout << "\tStep: " << stepCount++ << endl;
    //     cout << solutionSteps.at(i).queueSize << endl;
    // }
    // cout << endl;

    double currTimeMs = 0;

    cout << "times: " << endl;
    for (int i = 1; i < (int)solutionSteps.size() - 1; i++) {
        // cout << "\tStep: " << stepCount++ << endl;

        // chrono::duration<double> elapsedSeconds = solutionSteps.at(i).time;

        chrono::duration<double> elapsedSeconds = solutionSteps.at(i).time - solutionSteps.at(i - 1).time;

        double timeSeconds = elapsedSeconds.count();
        double timeMs = timeSeconds * 1000;
        currTimeMs += timeMs;
        cout << currTimeMs << endl;
    }
    
    cout << solutionSteps.back().queueSize << endl;
    
}



int main() {

    int testCases = 0;

    cout << "Enter testcase count" << endl;
    cin >> testCases;

    int puzzleLength = 0;
    cin >> puzzleLength;

    int testCase = 1;

    cout << endl;

    vector<vector<double>> timesForEachPuzzleMs(testCases);

    while (testCases--) {

        cout << testCases << endl;

        cout << "TEST CASE: " << testCase++ << endl;
        cout << "-------------------------------------" << endl;

        vector<vector<int>> puzzle(puzzleLength, vector<int>());

        string row;

        for (int i = 0; i < puzzleLength; i++) {
            cin >> row;

            stringstream ss(row);
            int val;

            for (int j = 0; j < puzzleLength; j++) {
                // puzzle.at(i).push_back(row.at(j) - '0');
                ss >> val;
                ss.get();
                puzzle.at(i).push_back(val);
            }
        }


        {
            // Uniform Cost Search
            auto startTime = chrono::high_resolution_clock::now();
            unordered_map<string, State> puzzleMap = uniformCostSearch(puzzle);
            auto endTime = chrono::high_resolution_clock::now();

            chrono::duration<double> elapsedSeconds = endTime - startTime;
            printTime(elapsedSeconds, 1);
            timesForEachPuzzleMs.at(testCase - 2).push_back(elapsedSeconds.count() * 1000);

            string solutionStateString = stateToString(getSolutionState(puzzleLength));

            if (puzzleMap.count(solutionStateString)) {
                vector<State> solutionSteps = traceBackSolutionSteps(puzzleMap, puzzleLength);
                reverse(solutionSteps.begin(), solutionSteps.end());

                cout << "Solution Path Length: " << solutionSteps.size() - 1 << endl;
                printSolution(solutionSteps, puzzle);

            } else {
                cout << "No Solution" << endl;
            }
        }

        cout << endl;

        {
            // Misplaced Tile Heuristic A-Star Search
            auto startTime = chrono::high_resolution_clock::now();
            unordered_map<string, State> puzzleMap = misplacedTileHeuristic(puzzle);
            auto endTime = chrono::high_resolution_clock::now();

            
            chrono::duration<double> elapsedSeconds = endTime - startTime;
            printTime(elapsedSeconds, 2);
            timesForEachPuzzleMs.at(testCase - 2).push_back(elapsedSeconds.count() * 1000);

            string solutionStateString = stateToString(getSolutionState(puzzleLength));

            if (puzzleMap.count(solutionStateString)) {
                vector<State> solutionSteps = traceBackSolutionSteps(puzzleMap, puzzleLength);
                reverse(solutionSteps.begin(), solutionSteps.end());
                printSolution(solutionSteps, puzzle);
                cout << "Solution Path Length: " << solutionSteps.size() - 1 << endl;


            } else {
                cout << "No Solution" << endl;
            }
        }

        cout << endl;

        {
            // Manhattan Distance Heuristic A-Star Search
            auto startTime = chrono::high_resolution_clock::now();
            unordered_map<string, State> puzzleMap = manhattanDistanceHeuristic(puzzle);
            auto endTime = chrono::high_resolution_clock::now();
            
            
            chrono::duration<double> elapsedSeconds = endTime - startTime;
            printTime(elapsedSeconds, 3);
            timesForEachPuzzleMs.at(testCase - 2).push_back(elapsedSeconds.count() * 1000);

            string solutionStateString = stateToString(getSolutionState(puzzleLength));

            if (puzzleMap.count(solutionStateString)) {
                vector<State> solutionSteps = traceBackSolutionSteps(puzzleMap, puzzleLength);
                reverse(solutionSteps.begin(), solutionSteps.end());

                cout << "Solution Path Length: " << solutionSteps.size() - 1 << endl;
                printSolution(solutionSteps, puzzle);

            } else {
                cout << "No Solution" << endl;
            }
        }

        cout << endl;

    }

    for (int i = 0; i < timesForEachPuzzleMs.size(); i++) {
        // cout << "Puzzle " << i + 1 << " Times: " << endl;
        cout << std::fixed << std::setprecision(2) << timesForEachPuzzleMs.at(i).at(0) << ", ";
        cout << std::fixed << std::setprecision(2) << timesForEachPuzzleMs.at(i).at(1) << ", ";
        cout << std::fixed << std::setprecision(2) << timesForEachPuzzleMs.at(i).at(2) << endl;
    }


    



    return 0;
}

