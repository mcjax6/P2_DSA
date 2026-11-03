//
// Created by mcafe on 11/3/2026.
//

#include "ListProcessor.h"

#include <algorithm>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

bool ListProcessor::openFile(string _filename) {


    ifstream ifs(_filename);
    if (ifs.is_open()) {
        compList.clear();
        string trash;
        getline(ifs, trash);

        string check;
        int randomIndex = 0;
        char trash2[1];
        while (getline(ifs, check, ',')) {
            int id = stoi(check);
            float s1;
            float s2;
            float s3;
            float s4;

            ifs >> s1;
            ifs.read(trash2, 1);
            ifs >> s2;
            ifs.read(trash2, 1);
            ifs >> s3;
            ifs.read(trash2, 1);
            ifs >> s4;
            ifs.read(trash2, 1);

            Competitor temp = Competitor(id, "bob", s1, s2, s3, s4);
            temp.updateScore(weight1, weight2, weight3, weight4);
            compList.push_back(temp);
        }

        std::sort(compList.begin(), compList.end(), [](const Competitor& a, const Competitor& b) {
            return a.composite_score > b.composite_score;
        });
        return true;
    }



    return false;
}

void ListProcessor::updateWeights(float w1, float w2, float w3, float w4) {

    weight1 = w1;
    weight2 = w2;
    weight3 = w3;
    weight4 = w4;

}

std::vector<Competitor> ListProcessor::getCompList() {
    return compList;
}