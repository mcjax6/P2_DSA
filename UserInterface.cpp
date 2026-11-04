//
// Created by fmflk on 11/3/2026.
//


#include "UserInterface.h"

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <limits>


// !!! IMPORTANT FOR LATER ME REMEMBER LOOK OOOOO CAPITALIZED
// REMEMBER TO CHANGE STUFF WHEN IT GETS FINALIZED!!!!!!!!!!!


//Store the CSV filename and initialize the random number generator
//This allows different tournament outcomes each time the program runs
UserInterface::UserInterface(const std::string& filename) {
    csvFile = filename;
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

//Ask the user for a whole number within a specified range
//If the input is invalid keep asking until a valid number is entered
bool UserInterface::readInt(const std::string& message, int lowest, int highest, int& answer) {
    while (true) {
        std::cout << message;
        if (!(std::cin >> answer)) {
            if (std::cin.eof()) {
                return false;
            }
            //Clear invalid input so the user can try again
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Please enter a whole number:\n";
        }
        else {
            //Remove any remaining characters from the input line
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (answer >= lowest && answer <= highest) {
                return true;
            }
            std::cout << "Please enter a number from " << lowest << " to " << highest << ":\n";
        }
    }
}

//Similar to readInt but allows decimal values
//Used when entering strength weights or the upset factor
bool UserInterface::readDouble(const std::string& message, double lowest, double highest, double& answer) {
    while (true) {
        std::cout << message;
        if (!(std::cin >> answer)) {
            if (std::cin.eof()) {
                return false;
            }
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Please enter a number:\n";
        }
        else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (std::isfinite(answer) && answer >= lowest && answer <= highest) {
                return true;
            }
            std::cout << "Please enter a number from " << lowest << " to " << highest << ":\n";
        }
    }
}

//Load competitor information using ListProcessor from other file
//Composite scores must be recalculated with the new weights
bool UserInterface::loadCompetitors() {
    processor.updateWeights(weights[0], weights[1], weights[2], weights[3]);
    if (!processor.openFile(csvFile)) {
        std::cout << "Could not load " << csvFile << ".\n";
        return false;
    }
    competitors = processor.getCompList();
    return !competitors.empty();
}

//Display a competitor's ID and name and composite score
void UserInterface::showCompetitor(int index) {
    const Competitor& c = competitors[index];
    std::cout << "Competitor #" << c.id;
    //FIX THIS LATER
    if (!c.name.empty() && c.name != "bob") {
        std::cout << " (" << c.name << ")";
    }
    std::cout << " | Score: " << std::fixed << std::setprecision(2) << c.composite_score;
}

//Display the program's main menu
//The user selects an operation by entering its corresponding number
void UserInterface::showMenu() {
    std::cout << "\n=== TOURNAMENT BRACKET PREDICTOR ===\n";
    std::cout << "1. Change Strength Weights\n";
    std::cout << "2. Search for Competitor\n";
    std::cout << "3. View Top Competitors\n";
    std::cout << "4. Build and Simulate a Bracket\n";
    std::cout << "5. Compare Tree Performance\n";
    std::cout << "6. Exit\n";
}

//Allow the user to change the importance of each strength category
//After updating the weights - reload the competitor list so that scores and rankings reflect new percentages
void UserInterface::changeWeights() {
    std::cout << "\nCurrent weights: ";
    for (int i = 0; i < 4; i++) {
        std::cout << weights[i] * 100 << "% ";
    }
    std::cout << "\nEnter four percentages that add to 100:\n";
    float newWeights[4];
    double sum = 0;
    for (int i = 0; i < 4; i++) {
        double percent;
        if (!readDouble("Strength " + std::to_string(i + 1) + " (%): ", 0, 100, percent)) {
            return;
        }
        //Convert a percentage into a decimal weight
        newWeights[i] = static_cast<float>(percent / 100.0);
        sum += percent;
    }
    //Reject weights that do not total 100%
    if (std::fabs(sum - 100.0) > 0.01) {
        std::cout << "Weights must add to 100%; nothing changed.\n";
        return;
    }
    //Save the previous weights in case reloading fails
    float oldWeights[4];
    for (int i = 0; i < 4; i++) {
        oldWeights[i] = weights[i];
        weights[i] = newWeights[i];
    }
    //Reload data and recalculate scores
    if (!loadCompetitors()) {
        for (int i = 0; i < 4; i++) {
            weights[i] = oldWeights[i];
        }
        processor.updateWeights(
            weights[0], weights[1], weights[2], weights[3]
        );
        std::cout << "Could not reload data; previous weights restored.\n";
    }
    else {
        std::cout << "Weights updated and competitors reranked.\n";
    }
}

//Ask which competitor the user wants to search for
// !!! The search will eventually call the Splay Tree or Red-Black Tree implemented later
//This part remains disconnected until those are done
void UserInterface::searchCompetitor() {
    std::cout << "Competitor name: ";
    std::string name;
    if (!std::getline(std::cin, name)) {
        return;
    }
    std::cout << "Search for '" << name << "' is not connected yet.\n";
    std::cout << "The team's tree search and unique names " << "are still needed.\n";
}

//Display the highest-ranked competitors
//We already sort competitors by their composite scores - so the first elements in the vector are the highest ranked
void UserInterface::showTopCompetitors() {
    int count;
    if (!readInt("How many top competitors? ", 1, static_cast<int>(competitors.size()), count)) {
        return;
    }
    //Limit printed records so the console stays readable
    int shown = count < 20 ? count : 20;
    std::cout << "\n--- TOP COMPETITORS ---\n";
    for (int i = 0; i < shown; i++) {
        std::cout << i + 1 << ". ";
        showCompetitor(i);
        std::cout << '\n';
    }
    if (shown < count) {
        std::cout << "Showing the first 20 of " << count << ".\n";
    }
}

//Arrange competitors into a seeded tournament bracket
//Matching the strongest competitor against the weakest, second strongest against second weakest, etc, etc
//This function arranges the seeds so higher-ranked competitors are also placed on different sides of the bracket
std::vector<int> UserInterface::makeSeedOrder(int bracketSize) {
    std::vector<int> seeds;
    seeds.push_back(1);
    while (static_cast<int>(seeds.size()) < bracketSize) {
        int newSize = static_cast<int>(seeds.size()) * 2;
        std::vector<int> nextSeeds;
        for (int i = 0; i < static_cast<int>(seeds.size()); i++) {
            nextSeeds.push_back(seeds[i]);
            nextSeeds.push_back(newSize + 1 - seeds[i]);
        }
        seeds = nextSeeds;
    }
    return seeds;
}

//Calculate the probability that a weaker competitor wins
// 1. Find the difference between composite scores
// 2. Divide the difference by the higher composite score
// 3. Subtract the result from 1
// 4. Multiply by the selected upset factor
double UserInterface::calculateUpset(double higherScore, double lowerScore, double upsetFactor) {
    //Avoid dividing by zero
    if (higherScore <= 0) {
        return 0.5;
    }
    double difference = higherScore - lowerScore;
    double upsetChance = (1.0 - difference / higherScore) * upsetFactor;
    //Keep the result between 0 and 1
    if (upsetChance < 0) {
        upsetChance = 0;
    }
    if (upsetChance > 1) {
        upsetChance = 1;
    }
    return upsetChance;
}

//Determine which of two competitors wins a matchup
//First, determine which competitor has the higher composite score - then calculate the probability of an upset - then generate a random decimal from 0 to 1
//If it is below the upset probability then the weaker competitor wins, otherwise the stronger competitor advances
int UserInterface::pickWinner(int first, int second, double upsetFactor, double& firstChance) {
    double score1 = competitors[first].composite_score;
    double score2 = competitors[second].composite_score;
    double randomNumber = std::rand() / (RAND_MAX + 1.0);
    //If both competitors have equal scores then give each competitor a 50% chance of winning
    if (score1 == score2) {
        firstChance = 0.5;
        if (randomNumber < 0.5) {
            return first;
        }
        else {
            return second;
        }
    }
    int stronger;
    int weaker;
    if (score1 > score2) {
        stronger = first;
        weaker = second;
    }
    else {
        stronger = second;
        weaker = first;
    }
    double high = competitors[stronger].composite_score;
    double low = competitors[weaker].composite_score;
    double upsetChance = calculateUpset(high, low, upsetFactor);

    //Record the first competitor's win probability so it can be displayed in bracket
    if (first == stronger) {
        firstChance = 1.0 - upsetChance;
    }
    else {
        firstChance = upsetChance;
    }
    if (randomNumber < upsetChance) {
        return weaker;
    }
    else {
        return stronger;
    }
}

//Build and simulate the tournament
// 1. Select the top X competitors
// 2. Arrange the first round using seeded matchups
// 3. Compare pairs of competitors
// 4. Store each winner in nextRound
// 5. Replace currentRound with nextRound
// 6. Repeat until one competitor remains
void UserInterface::simulateTournament() {
    int count;
    if (!readInt("How many competitors (2 or more)? ", 2, static_cast<int>(competitors.size()), count)) {
        return;
    }
    double upsetFactor;
    if (!readDouble("Upset factor (0 to 1): ", 0, 1, upsetFactor)) {
        return;
    }

    //Single-elimination brackets normally use powers of 2
    //If the chosen number is not a power of 2 then additional bracket slots will be treated as byes
    int bracketSize = 1;
    while (bracketSize < count) {
        bracketSize *= 2;
    }
    std::vector<int> seeds = makeSeedOrder(bracketSize);
    std::vector<int> currentRound;

    //Store indices into the sorted competitor vector (a value of -1 represents an empty bracket slot - a bye)
    for (int i = 0; i < bracketSize; i++) {
        if (seeds[i] <= count) {
            currentRound.push_back(seeds[i] - 1);
        }
        else {
            currentRound.push_back(-1);
        }
    }
    int round = 1;

    //Continue simulating until only the champion is left
    while (currentRound.size() > 1) {
        std::cout << "\n--- ROUND " << round << " ---\n";
        std::vector<int> nextRound;
        int matches = 0;

        //Pair competitors at indices
        //Increase by 2 so competitors are not reused within the same round
        for (int i = 0; i < static_cast<int>(currentRound.size()); i += 2) {
            int first = currentRound[i];
            int second = currentRound[i + 1];
            int winner;
            double firstChance = 1.0;

            //A bye automatically advances the other competitor
            if (first == -1) {
                winner = second;
            }
            else if (second == -1) {
                winner = first;
            }
            else {
                winner = pickWinner(first, second, upsetFactor, firstChance);
            }

            //Store the winner for the next rounds
            nextRound.push_back(winner);

            //Simulate all matches - but display only the first
            //16 per round
            if (matches < 16) {
                std::cout << "Match " << i / 2 + 1 << ": ";
                if (first == -1 || second == -1) {
                    showCompetitor(winner);
                    std::cout << " advances by bye\n";
                }
                else {
                    std::cout << "#" << competitors[first].id << " (" << std::fixed << std::setprecision(1) << firstChance * 100 << "%) vs #" << competitors[second].id << " (" << (1 - firstChance) * 100 << "%)\n";
                    std::cout << "  Winner: ";
                    showCompetitor(winner);
                    std::cout << '\n';
                }
            }
            matches++;
        }

        if (matches > 16) {
            std::cout << matches - 16 << " more matches were simulated.\n";
        }

        //The winners of this round become the competitors for the next round
        currentRound = nextRound;
        round++;
    }

    //Once only one competitor remains - the tournament ends
    std::cout << "\n=== TOURNAMENT CHAMPION ===\n";
    showCompetitor(currentRound[0]);
    std::cout << '\n';
}

//This will eventually display the execution time comparisons between the two data structures
// !!! Actual timing results will have to come from the tree implementations
void UserInterface::showBenchmarks() {
    std::cout << "Tree performance measurements " << "are not connected yet.\n";
}

//Main program loop.
//Load the dataset, display the menu, and execute the operation selected by the user
//Continue until exit
void UserInterface::run() {
    if (!loadCompetitors()) {
        return;
    }
    std::cout << "Loaded " << competitors.size() << " competitors.\n";
    while (true) {
        showMenu();
        int choice;
        if (!readInt("Choose an option (1-6): ", 1, 6, choice)) {
            return;
        }
        if (choice == 1) {
            changeWeights();
        }
        else if (choice == 2) {
            searchCompetitor();
        }
        else if (choice == 3) {
            showTopCompetitors();
        }
        else if (choice == 4) {
            simulateTournament();
        }
        else if (choice == 5) {
            showBenchmarks();
        }
        else if (choice == 6) {
            std::cout << "Goodbye!\n";
            return;
        }
    }
}

