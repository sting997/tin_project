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
    ConsoleMenu() : msg("Select an option: ") {}

    ConsoleMenu(std::string omsg) : msg(omsg) {}

    void add(int val, std::string opt) {
        values.push_back(val);
        options.push_back(opt);
    }

    int opt() { return option; }

    bool selection();

    std::ostream &display();

private:
    int option;
    std::string msg;
    std::list<std::string> options;
    std::list<int> values;
};


#endif //TIN_MENU_H
