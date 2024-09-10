/***
 * @brief
 * @author Johann Fotsing
 * @date 2024-09-10
 * @file pert.cpp
 */

#include <iostream>
#include <pert.h>
#include "../depends/rapidxml-1.13/rapidxml.hpp"

using namespace pert;
using namespace rapidxml;

template class activity_t<int>;
template class network_t<int, int>;

int main()
{
    // Dummy network
    pert::network_t<int, int> test;
    test.add_activity(1, 3, 3)
        .add_activity(1, 2, 1)
        .add_activity(2, 4, 1)
        .add_activity(3, 4, 6)
        .schedule(0, 9);
    // List activities
    for (const auto& a: test.activities())
    {
        std::cout << a.trigger_event << "--->" << a.completion_event << "  : " << test.estimated_duration(a) << std::endl;
    }
    // List initial events
    std::cout << "Initial events: ";
    for (const auto& e: test.initial_events())
    {
        std::cout << e << " ";
    }
    std::cout << std::endl;
    // List terminal events
    std::cout << "Terminal events: ";
    for (const auto& e: test.terminal_events())
    {
        std::cout << e << " ";
    }
    std::cout << std::endl;
    // Is well formed ?
    std::cout << "Well formed: " << test.is_well_formed() << std::endl;
    // Earliest finish
    std::cout << "Earliest finish: " << test.earliest_occurence(4) << std::endl;
    // Latest start
    std::cout << "Latest start: " << test.latest_occurence(1) << std::endl;

    return 0;
}