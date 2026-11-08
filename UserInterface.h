//
// Created by fmflk on 11/3/2026.
//

#ifndef P2_DSA_USERINTERFACE_H
#define P2_DSA_USERINTERFACE_H

#include "ListProcessor.h"
#include <string>
#include <vector>
#include "SplayTree.h"
#include "RedBlackTree.h"

class UserInterface {
private:
    //Uses the existing class to load and sort competitors
    ListProcessor processor;

    //Stores competitors in order of composite score
    std::vector<Competitor> competitors;

    //Location of the CSV file
    std::string csvFile;

    //Currently uses four strength categories
    //Each strength initially contributes 25% to the total
    float weights[4] = {0.25f, 0.25f, 0.25f, 0.25f};

    //Both trees store competitors using unique IDs
    RedBlackTree redBlackTree;
    SplayTree splayTree;

    bool treesBuilt = false;

    //Save insertion times for performance comparisons
    double redBlackBuildMs = 0.0;
    double splayBuildMs = 0.0;

    //Loading and user input
    bool loadCompetitors();
    bool readInt(const std::string& prompt, int minimum, int maximum, int& value);
    bool readDouble(const std::string& prompt, double minimum, double maximum, double& value);

    //Search for and display competitors
    Competitor findById(int id, int treeChoice);
    void showCompetitor(int id, int treeChoice);
    bool chooseTree(int& treeChoice);

    //Main menu operations
    void showMenu();
    void changeWeights();
    void searchCompetitor();
    void showTopCompetitors();
    void simulateTournament();
    void showBenchmarks();

    //Functions used to construct and simulate bracket
    std::vector<int> makeSeedOrder(int bracketSize);

    double calculateUpset(double highScore, double lowScore, double upsetFactor);
    int pickWinner(int firstID, int secondID, double upsetFactor, double& firstChance, int treeChoice);

    //Measure tree search performance
    double timeRedBlackSearch(const std::vector<int>& ids, int& found);
    double timeSplaySearch(const std::vector<int>& ids, int& found);

public:
    explicit UserInterface(const std::string& filename);

    //Starts the menu and handles user choices
    void run();
};

#endif //P2_DSA_USERINTERFACE_H
