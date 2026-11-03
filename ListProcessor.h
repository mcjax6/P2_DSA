//
// Created by mcafe on 11/3/2026.
//

#ifndef P2_DSA_LISTPROCESSOR_H
#define P2_DSA_LISTPROCESSOR_H
#include <string>
#include <vector>
#include <fstream>
#include "Competitor.h"

class ListProcessor {

    std::vector<Competitor> compList;
    float weight1 = 0;
    float weight2 = 0;
    float weight3 = 0;
    float weight4 = 0;

public:

    bool openFile(std::string _filename);
    void updateWeights(float w1, float w2, float w3, float w4);
    std::vector<Competitor> getCompList();

};


#endif //P2_DSA_LISTPROCESSOR_H
