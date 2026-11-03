#include <iostream>
#include <vector>
#include <string>
#include "Competitor.h"
#include "ListProcessor.h"

int main() {

    ListProcessor big = ListProcessor();
    big.updateWeights(1, 1, 1, 1);
    big.openFile("generated_data_100k.csv");
    std::vector<Competitor> temp = big.getCompList();

    for (auto i = 0; i < temp.size(); i++) {
        std::cout << "ID: " << temp[i].id << std::endl;
        std::cout << "Score: " << temp[i].composite_score << std::endl;
    }
    
}