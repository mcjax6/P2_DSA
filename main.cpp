#include <iostream>
#include <vector>
#include <string>
#include "Competitor.h"
#include "ListProcessor.h"
#include "RedBlackTree.h"

int main() {

    ListProcessor big = ListProcessor();
    big.updateWeights(1, 1, 1, 1);
    big.openFile("generated_data_100k.csv");
    std::vector<Competitor> temp = big.getCompList();

    RedBlackTree comps;

    for (auto i = 0; i < temp.size(); i++) {
        comps.insert(temp[i]);
    }

    Competitor random = comps.search(500);
    std::cout << random.id << std::endl;
    
}