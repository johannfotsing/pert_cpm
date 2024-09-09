/**
 * @brief
 * 
 * @author Johann Fotsing
 * @date 2024-09-08
 * @file event.cpp
 */

#include "../include/event.h"

using namespace pert_cpm;

void event::set_id(const int16_t& an_id)
{
    _id = an_id;
}

void event::set_name(const std::string& a_name)
{
    _name = a_name;
}

bool event::operator==(const event& an_event)
{
    return this->_id == an_event._id or this->_name == an_event._name;
}