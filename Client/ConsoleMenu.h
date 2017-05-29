//
// Created by riuoku on 29.05.17.
//

#ifndef TIN_CONSOLEMENU_H
#define TIN_CONSOLEMENU_H

#include <iostream>
#include <cstdlib>
#include <string>
#include <list>
#include <algorithm>

class ConsoleMenu {
public:
    // Generic prompt for input
    ConsoleMenu() : msg("Select an option: ") {}

    // Custom prompt, just in case
    ConsoleMenu(std::string omsg) : msg(omsg) {}

    // The order of 'values' doesn't matter, 'options' does.
    void add(int val, std::string opt) {
        values.push_back(val);
        options.push_back(opt);
    }

    // For switch statements and the like
    int opt() { return option; }

    // Interactive input
    bool selection();

    // Return ostream& for linked output (e.g. obj.display()<<endl;)
    std::ostream &display();

private:
    int option;  // Interactive input option
    std::string msg;     // Prompt message for input
    std::list<std::string> options; // List of printable options
    std::list<int> values;  // List of valid option values
};


#endif //TIN_MENU_H
