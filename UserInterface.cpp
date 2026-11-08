//
// Created by fmflk on 11/3/2026.
//


#include "UserInterface.h"

#include <chrono>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <limits>


//Insert a competitor into the Splay Tree
void SplayTree::insert(const Competitor& comp) {
    //If the tree is empty, create the first node
    if (root == nullptr) {
        root = new Node(comp);
        return;
    }
    //Bring the closest matching node to the root
    root = splay(root, comp.name);
    //If this competitor already exists, update its information
    if (root->data.name == comp.name) {
        root->data = comp;
        return;
    }
    //Create a new node for the competitor
    Node* newNode = new Node(comp);
    //If the new key is smaller, place it before the old root
    if (comp.name < root->data.name) {
        newNode->left = root->left;
        newNode->right = root;
        root->left = nullptr;
    }
    //Otherwise, place it after the old root
    else {
        newNode->right = root->right;
        newNode->left = root;
        root->right = nullptr;
    }
    //The inserted competitor becomes the new root
    root = newNode;
}

//Search for a competitor using its string key
Competitor SplayTree::search(const std::string& key) {
    //Search and move the closest matching node to the root
    root = splay(root, key);
    //Return the competitor if the key was found
    if (root != nullptr && root->data.name == key) {
        return root->data;
    }
    //Return an empty competitor if the key was not found
    return Competitor();
}

//Store the CSV filename and initialize the random number generator
//This allows different tournament outcomes each time the program runs
UserInterface::UserInterface(const std::string& filename) {
    csvFile = filename;
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

//Ask the user for a whole number within a specified range
//If the input is invalid keep asking until a valid number is entered
bool UserInterface::readInt(const std::string& prompt, int minimum, int maximum, int& value) {
    while (true) {
        std::cout << prompt;
        if (!(std::cin >> value)) {
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
            if (value >= minimum && value <= maximum) {
                return true;
            }
            std::cout << "Please enter a number from " << minimum << " to " << maximum << ":\n";
        }
    }
}

//Similar to readInt but allows decimal values
//Used when entering strength weights or the upset factor
bool UserInterface::readDouble(const std::string& prompt, double minimum, double maximum, double& value) {
    while (true) {
        std::cout << prompt;
        if (!(std::cin >> value)) {
            if (std::cin.eof()) {
                return false;
            }
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Please enter a number:\n";
        }
        else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (std::isfinite(value) && value >= minimum && value <= maximum) {
                return true;
            }
            std::cout << "Please enter a number from " << minimum << " to " << maximum << ":\n";
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
    if (competitors.empty()) {
        std::cout << "No competitors were loaded.\n";
        return false;
    }
    if (!treesBuilt) {
        //Measure how long Red-Black Tree insertion takes
        auto start = std::chrono::steady_clock::now();
        for (int i = 0; i < static_cast<int>(competitors.size()); i++) {
            redBlackTree.insert(competitors[i]);
        }
        auto finish = std::chrono::steady_clock::now();
        redBlackBuildMs =
            std::chrono::duration<double, std::milli>(finish - start).count();

        //The Splay Tree compares competitor names
        //Use the unique ID converted to a string
        start = std::chrono::steady_clock::now();
        for (int i = 0; i < static_cast<int>(competitors.size()); i++) {
            Competitor item = competitors[i];
            item.name = std::to_string(item.id);
            splayTree.insert(item);
        }
        finish = std::chrono::steady_clock::now();
        splayBuildMs = std::chrono::duration<double, std::milli>(finish - start).count();
        treesBuilt = true;
    }
    return true;
}

//Both trees store the same competitors but use different keys
//Red-Black Tree: integer ID
//Splay Tree: string representation of ID
// Recalculate the score on the returned copy so changing weights - works without rebuilding either tree
Competitor UserInterface::findById(int id, int treeChoice) {
    Competitor result;
    if (treeChoice == 2) {
        result = splayTree.search(std::to_string(id));
    }
    else {
        result = redBlackTree.search(id);
    }
    if (id > 0 && result.id == id) {
        result.updateScore(
            weights[0], weights[1], weights[2], weights[3]
        );
    }
    return result;
}

//Display a competitor's ID and name and composite score
void UserInterface::showCompetitor(int id, int treeChoice) {
    Competitor found = findById(id, treeChoice);
    if (id <= 0 || found.id != id) {
        std::cout << "Competitor #" << id << " not found";
        return;
    }
    std::cout << "Competitor #" << found.id;
    //Hide the placeholder name from the CSV
    if (!found.name.empty() && found.name != "bob" && found.name != std::to_string(found.id)) {
        std::cout << " (" << found.name << ")";
    }
    std::cout << " | Score: " << std::fixed << std::setprecision(2) << found.composite_score;
}

//Ask which of the two data structures to use.
bool UserInterface::chooseTree(int& treeChoice) {
    return readInt("Choose tree (1 = Red-Black, 2 = Splay): ", 1, 2, treeChoice);
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
void UserInterface::searchCompetitor() {
    int id;
    if (!readInt("Competitor ID: ",1,std::numeric_limits<int>::max(),id)) {
        return;
    }
    int treeChoice;
    if (!chooseTree(treeChoice)) return;
    Competitor result = findById(id, treeChoice);
    if (result.id != id) {
        std::cout << "Competitor not found.\n";
        return;
    }
    showCompetitor(id, treeChoice);
    std::cout << '\n';
}

//Display the highest-ranked competitors
//We already sort competitors by their composite scores - so the first elements in the vector are the highest ranked
void UserInterface::showTopCompetitors() {
    int count;
    if (!readInt("How many top competitors? ",1,static_cast<int>(competitors.size()),count)) {
        return;
    }
    int treeChoice;
    if (!chooseTree(treeChoice)) return;
    //Print at most 20 competitors to avoid huge console output
    int shown = count < 20 ? count : 20;
    std::cout << "\n--- TOP COMPETITORS ---\n";
    for (int i = 0; i < shown; i++) {
        std::cout << i + 1 << ". ";
        showCompetitor(competitors[i].id, treeChoice);
        std::cout << '\n';
    }
    if (shown < count) {
        std::cout << "Showing " << shown << " of " << count << ".\n";
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
int UserInterface::pickWinner(int firstID, int secondID, double upsetFactor, double& firstChance, int treeChoice) {
    Competitor first = findById(firstID, treeChoice);
    Competitor second = findById(secondID, treeChoice);
    double score1 = first.composite_score;
    double score2 = second.composite_score;
    double roll = std::rand() / (RAND_MAX + 1.0);
    //Equal scores give each competitor a 50% chance
    if (score1 == score2) {
        firstChance = 0.5;
        if (roll < 0.5) {
            return firstID;
        } else {
            return secondID;
        }
    }
    int stronger;
    int weaker;
    if (score1 > score2) {
        stronger = firstID;
        weaker = secondID;
    } else {
        stronger = secondID;
        weaker = firstID;
    }
    double high = (score1 > score2) ? score1 : score2;
    double low = (score1 > score2) ? score2 : score1;
    double upsetChance = calculateUpset(high, low, upsetFactor);
    //Save the first competitor's win chance for display
    if (firstID == stronger) {
        firstChance = 1.0 - upsetChance;
    } else {
        firstChance = upsetChance;
    }
    //The weaker competitor wins if an upset occurs
    if (roll < upsetChance) {
        return weaker;
    } else {
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
    int treeChoice;
    if (!chooseTree(treeChoice)) {
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

    //Store actual competitor IDs in the bracket
    //A value of -1 represents an empty slot (bye)
    for (int i = 0; i < bracketSize; i++) {
        if (seeds[i] <= count) {
            currentRound.push_back(competitors[seeds[i] - 1].id);
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
                winner = pickWinner(first, second, upsetFactor, firstChance, treeChoice);
            }

            //Store the winner for the next rounds
            nextRound.push_back(winner);

            //Simulate all matches - but display only the first
            //16 per round
            if (matches < 16) {
                std::cout << "Match " << i / 2 + 1 << ": ";
                if (first == -1 || second == -1) {
                    showCompetitor(winner, treeChoice);
                    std::cout << " advances by bye\n";
                }
                else {
                    std::cout << "#" << first << " (" << std::fixed << std::setprecision(1) << firstChance * 100 << "%) vs #" << second << " (" << (1 - firstChance) * 100 << "%)\n";
                    std::cout << "  Winner: ";
                    showCompetitor(winner, treeChoice);
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
    showCompetitor(currentRound[0], treeChoice);
    std::cout << '\n';
}

//Time Red-Black Tree searches
//Measure lookup operations, not console output
double UserInterface::timeRedBlackSearch(const std::vector<int>& ids, int& found) {
    found = 0;
    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < static_cast<int>(ids.size()); i++) {
        Competitor result = redBlackTree.search(ids[i]);
        if (result.id == ids[i]) {
            found++;
        }
    }
    auto finish = std::chrono::steady_clock::now();
    return std::chrono::duration<double, std::milli>(finish - start).count();
}

//Time Splay Tree searches using the same competitor IDs
//Convert IDs to strings BEFORE timing so the measured section focuses on the tree's search operations.
double UserInterface::timeSplaySearch(const std::vector<int>& ids, int& found) {
    found = 0;
    std::vector<std::string> keys;
    for (int i = 0; i < static_cast<int>(ids.size()); i++) {
        keys.push_back(std::to_string(ids[i]));
    }
    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < static_cast<int>(keys.size()); i++) {
        Competitor result = splayTree.search(keys[i]);
        if (result.id == ids[i]) {
            found++;
        }
    }
    auto finish = std::chrono::steady_clock::now();
    return std::chrono::duration<double, std::milli>(finish - start).count();
}

//This will  display the execution time comparisons between the two data structures
void UserInterface::showBenchmarks() {
    const int queries = 10000;
    int popularCount = (competitors.size() < 50) ? static_cast<int>(competitors.size()) : 50;
    std::vector<int> randomIds;
    std::vector<int> repeatedIds;
    for (int i = 0; i < queries; i++) {
        randomIds.push_back(competitors[std::rand() % competitors.size()].id);
        repeatedIds.push_back(competitors[std::rand() % popularCount].id);
    }
    //Count successful searches to verify lookup results
    int redRandomFound;
    int redRepeatedFound;
    int splayRandomFound;
    int splayRepeatedFound;

    double redRandomMs = timeRedBlackSearch(randomIds, redRandomFound);
    double redRepeatedMs = timeRedBlackSearch(repeatedIds, redRepeatedFound);
    double splayRandomMs = timeSplaySearch(randomIds, splayRandomFound);
    double splayRepeatedMs = timeSplaySearch(repeatedIds, splayRepeatedFound);

    std::cout << "\n--- TREE PERFORMANCE ---\n";
    std::cout << "Records: " << competitors.size() << '\n';
    std::cout << "Searches per workload: " << queries << '\n';

    std::cout << std::fixed << std::setprecision(3);

    std::cout << "Red-Black insertion: " << redBlackBuildMs << " ms\n";
    std::cout << "Splay insertion: " << splayBuildMs << " ms\n";
    std::cout << "Red-Black random searches: " << redRandomMs << " ms (" << redRandomFound << "/" << queries << " found)\n";
    std::cout << "Splay random searches: " << splayRandomMs << " ms (" << splayRandomFound << "/" << queries << " found)\n";
    std::cout << "Red-Black repeated searches: " << redRepeatedMs << " ms (" << redRepeatedFound << "/" << queries << " found)\n";
    std::cout << "Splay repeated searches: " << splayRepeatedMs << " ms (" << splayRepeatedFound << "/" << queries << " found)\n";

    //These are node-size estimates, not total program memory
    std::cout << "Red-Black node memory (estimate): " << sizeof(RedBlackNode) * competitors.size() << " bytes\n";
    std::cout << "Splay node memory (estimate): " << sizeof(Node) * competitors.size() << " bytes\n";
    std::cout << "Note: Splay searches use string IDs; " << "Red-Black uses integer IDs.\n";
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

