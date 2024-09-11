/***
 * @brief
 * @author Johann Fotsing
 * @date 2024-09-10
 * @file pert.cpp
 */

#include <iostream>
#include <pert.h>
#include <fstream>
#include <sstream>
#include <streambuf>
#include "../depends/rapidxml-1.13/rapidxml.hpp"

using namespace pert;
using namespace rapidxml;

template class activity_t<int>;
template class network_t<int, int>;

using network = network_t<int, int>;

// Test functions
int test_basic(const network&);

int main(int argc, char** argv)
{
    network test_network;

    // Dummy network
    /*test_network.add_activity(1, 3, 3)
        .add_activity(1, 2, 1)
        .add_activity(2, 4, 1)
        .add_activity(3, 4, 6)
        .schedule(0, 9);*/

    // from txt network
    std::ifstream input_file(argv[1]);
    std::string file_str((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
    input_file.close();
    test_network = network::from_txt(file_str);

    return test_basic(test_network);
    
}

int test_basic(const network& a_network)
{
    // List activities
    for (const auto& a: a_network.activities())
    {
        std::cout << a.trigger_event << "--->" << a.completion_event << "  : " << a_network.estimated_duration(a) << std::endl;
    }
    // List initial events
    std::cout << "Initial events: ";
    for (const auto& e: a_network.initial_events())
    {
        std::cout << e << " ";
    }
    std::cout << std::endl;
    // List terminal events
    std::cout << "Terminal events: ";
    for (const auto& e: a_network.terminal_events())
    {
        std::cout << e << " ";
    }
    std::cout << std::endl;
    // Is well formed ?
    std::cout << "Well formed: " << a_network.is_well_formed() << std::endl;
    // Earliest finish
    std::cout << "Earliest finish: " << a_network.earliest_occurence(4) << std::endl;
    // Latest start
    std::cout << "Latest start: " << a_network.latest_occurence(1) << std::endl;

    return 0;
}