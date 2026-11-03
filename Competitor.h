#ifndef P2_DSA_COMPETITOR_H
#define P2_DSA_COMPETITOR_H

#include <string>


struct Competitor {
    int id;
    std::string name;

    float strength_1;
    float strength_2;
    float strength_3;
    float strength_4;

    float composite_score;

    Competitor() : id(0), name(""), strength_1(0.0f), strength_2(0.0f), strength_3(0.0f), strength_4(0.0f), composite_score(0.0f) {}

    Competitor(int id, std::string name, float s1, float s2, float s3, float s4) : id(id), name(name), strength_1(s1), strength_2(s2), strength_3(s3), strength_4(s4), composite_score(0.0f) {}

    void updateScore(float weight1, float weight2, float weight3, float weight4) {
        composite_score = (strength_1 * weight1) + (strength_2 * weight2) + (strength_3 * weight3) + (strength_4 * weight4);
    }

    bool operator<(const Competitor& other) const {
        return id < other.id;
    }

    bool operator>(const Competitor& other) const {
        return id > other.id;
    }

    bool operator==(const Competitor& other) const {
        return id == other.id;
    }
};


#endif
