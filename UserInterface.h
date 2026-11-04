//
// Created by fmflk on 11/3/2026.
//

#ifndef P2_DSA_USERINTERFACE_H
#define P2_DSA_USERINTERFACE_H

#include "ListProcessor.h"
#include <string>
#include <vector>

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

    //Loading and user input
    bool loadCompetitors();
    bool readInt(const std::string& message, int lowest, int highest, int& answer);
    bool readDouble(const std::string& message, double lowest, double highest, double& answer);

    //Main menu operations
    void showMenu();
    void changeWeights();
    void searchCompetitor();
    void showTopCompetitors();
    void simulateTournament();
    void showBenchmarks();
    void showCompetitor(int index);

    //Functions used to construct and simulate bracket
    std::vector<int> makeSeedOrder(int bracketSize);

    double calculateUpset(double higherScore, double lowerScore, double upsetFactor);

    int pickWinner(int first, int second, double upsetFactor, double& firstChance);

public:
    UserInterface(const std::string& filename);

    //Starts the menu and handles user choices
    void run();
};

#endif //P2_DSA_USERINTERFACE_H
