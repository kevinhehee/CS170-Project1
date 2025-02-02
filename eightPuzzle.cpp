#include <bits/stdc++.h>
#include <iostream>
#include <unordered_set>

using namespace std;

bool uniformCostSearch(vector<vector<int> > puzzle);
pair<int,int> findEmptyCoord(vector<vector<int> > &puzzle);
void misplacedTileHeuristic(vector<vector<int> > &puzzle);
void manhattanDistanceHeuristic(vector<vector<int> > &puzzle);
int countMisplacedTiles(vector<vector<int> > &puzzle);
void printPuzzle(vector<vector<int> > &puzzle);
bool isSolutionState(vector<vector<int> > &state);
bool inRange(int row, int col, int size);
string stateToString(const vector<vector<int>> &puzzle);

string stateToString(vector<vector<int>> &puzzle) {
    string result;

    for (int r = 0; r < puzzle.size(); r++) {
        for (int c = 0; c < puzzle.at(0).size(); c++) {
            result.push_back(puzzle.at(r).at(c));
        }
    }
    return result;
}


vector<int> directions{-1, 0, 1, 0, -1};

bool uniformCostSearch(vector<vector<int> > puzzle) {
    queue<vector<vector<int> > > nodes;

    unordered_set<string> visited;

    visited.insert(stateToString(puzzle));
    nodes.push(puzzle);

    int puzzleNumber = 1;
    int duplicates = 1;

    while (!nodes.empty()) {
        vector<vector<int> > currNode = nodes.front();
        nodes.pop();
        
        // cout << puzzleNumber++ << endl;
        // printPuzzle(currNode);
        // cout << endl;

        if (isSolutionState(currNode)) {
            return true;
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
                if (visited.find(newStateString) == visited.end()) {
                    nodes.push(currNode);
                    visited.insert(newStateString);
                }
                swap(currNode.at(zeroRow).at(zeroCol), currNode.at(newRow).at(newCol));
            }
        }
    }

    return false;


}

void misplacedTileHeuristic(vector<vector<int> > &puzzle) {


}

void manhattanDistanceHeuristic(vector<vector<int> > &puzzle) {

}

bool inRange(int row, int col, int size) {
    if (row >= 0 && col >= 0 && row < size && col < size) {
        return true;
    }
    return false;
}

pair<int,int> findEmptyCoord(vector<vector<int> > &puzzle) {
    for (int r = 0; r < puzzle.size(); r++) {
        for (int c = 0; c < puzzle.at(0).size(); c++) {
            if (puzzle.at(r).at(c) == 0) {
                return {r, c};
            }
        }
    }
    return {-1, -1};
}


int countMisplacedTiles(vector<vector<int> > &puzzle) {

    

    return 0;

}

void printPuzzle(vector<vector<int> > &puzzle) {
    for (int r = 0; r < puzzle.size(); r++) {
        for (int c = 0; c < puzzle.at(0).size(); c++) {
            cout << puzzle.at(r).at(c);
        }
        cout << endl;
    }
    cout << endl;
}

bool isSolutionState(vector<vector<int> > &state) {

    int currNumber = 1;

    int maxNumber = state.size() * state.size();

    for (int row = 0; row < state.size(); row++) {
        for (int col = 0; col < state.at(0).size(); col++) {

            if (currNumber == maxNumber) {
                break;
            }

            if (state.at(row).at(col) != currNumber) {
                return false;
            }

            currNumber++;
        }
    }


    return true;
}


int main() {

    int testCases = 0;

    cout << "Enter testcase count" << endl;
    cin >> testCases;

    int puzzleWidth = 0;
    cin >> puzzleWidth;

    int testCase = 1;

    cout << endl << endl;
    while (testCases--) {

        cout << "TEST CASE: " << testCase++ << endl;
        cout << "-------------------------------------" << endl;

        vector<vector<int> > puzzle(puzzleWidth, vector<int>());

        string row;

        for (int i = 0; i < puzzleWidth; i++) {
            cin >> row;

            for (int j = 0; j < row.size(); j++) {
                puzzle.at(i).push_back(row.at(j) - '0');
            }


        }

        auto start = chrono::high_resolution_clock::now();

        bool validSolution = uniformCostSearch(puzzle);

        auto end = chrono::high_resolution_clock::now();

        chrono::duration<double> elapsedSeconds = end - start;

        double timeSeconds = elapsedSeconds.count();
        double timeMs = timeSeconds * 1000;

        cout << "Uniform Cost Search: " << endl;
        cout << "Time to search: " << timeSeconds << " seconds" << endl;
        cout  << timeMs << " ms" << endl;






        // if (validSolution) {
        //     cout << "Solution" << endl;
        // } else {
        //     cout << "No Solution" << endl;
        // }

        cout << endl << endl;








    }



    return 0;
}

