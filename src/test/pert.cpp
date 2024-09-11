/***
 * @brief
 * @author Johann Fotsing
 * @date 2024-09-10
 * @file pert.cpp
 */

#include <iostream>
#include "../include/test/pert.h"
#include <fstream>
#include <sstream>
#include <streambuf>
#include "../depends/rapidxml-1.13/rapidxml.hpp"

int test_from_dummy()
{
    network test_network;

    // Dummy network
    /*
    1--->2  : 2
    1--->4  : 2
    1--->7  : 1
    2--->3  : 4
    4--->5  : 5
    3--->6  : 1
    4--->8  : 8
    5--->6  : 4
    7--->8  : 3
    6--->9  : 3
    8--->9  : 5
    */
    test_network.add_activity(1, 2, 2)
        .add_activity(1, 4, 2)
        .add_activity(1, 7, 1)
        .add_activity(2, 3, 4)
        .add_activity(4, 5, 5)
        .add_activity(3, 6, 1)
        .add_activity(4, 8, 8)
        .add_activity(5, 6, 4)
        .add_activity(7, 8, 3)
        .add_activity(6, 9, 3)
        .add_activity(8, 9, 5)
        .schedule(0, 21);

    return test_basic(test_network);
}

int test_from_txt(const char* file_name)
{
    // from txt network
    std::ifstream input_file(file_name);
    std::string file_str((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
    input_file.close();
    network test_network = network::from_txt(file_str);
    test_network.schedule(0, 21);
    return test_basic(test_network);
}

int test_basic(const network& a_network)
{
    // Network display section
    std::cout << "\n* Network\n----------" << std::endl;
    std::cout << "Initial events: ";
    for (const auto& e: a_network.initial_events())
    {
        std::cout << e << " ";
    }
    std::cout << std::endl;
    std::cout << "Terminal events: ";
    for (const auto& e: a_network.terminal_events())
    {
        std::cout << e << " ";
    }
    std::cout << std::endl;
    std::cout << "Well formed: " << a_network.is_well_formed() << std::endl;
    std::cout << "----------" << std::endl;
    // List activities
    for (const auto& a: a_network.activities())
    {
        std::cout << a.trigger_event << "--->" << a.completion_event << "  : " << a_network.estimated_duration(a) << std::endl;
    }
    std::cout << std::endl;

    // Schedule section
    std::cout << "* Schedule\n----------" << std::endl;
    std::cout << "Initial time: " << a_network.initial_time() << std::endl;
    std::cout << "Terminal time: " << a_network.terminal_time() << std::endl;
    std::cout << "Earliest finish: " << a_network.earliest_occurence(*a_network.terminal_events().begin()) << std::endl;
    std::cout << "Latest start: " << a_network.latest_occurence(*a_network.initial_events().begin()) << std::endl;
    std::cout << std::endl;

    return 0;
}