#include <iostream>
#include <vector>
#include <string>
#include "Competitor.h"
#include "ListProcessor.h"
#include "RedBlackTree.h"
#include "UserInterface.h"

int main() {

    //Create the user interface using the competitor dataset
    UserInterface ui("generated_data_100k.csv");

    //Start the program and display the main menu
    ui.run();

    return 0;
}