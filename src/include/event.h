/***
 * @brief This file contains the definition of class @code{event}.
 * 
 * @author Johann Fotsing
 * @date 2024-09-08
 * @file event.h
 */

#pragma once

#include <ctime>
#include <chrono>
#include <string>

namespace pert_cpm
{

    //using namespace std;

    using datetime_t = std::chrono::time_point<std::chrono::system_clock>;
    using time_t = std::time_t;

    /**
     * This class defines the @code{event} class.
     * 
     * Class specification:
     * 
     */
    class event
    {
        // Constructor(s)
        event();
        event(const int& id, const std::string& name) : _id(id), _name(name) {};

        friend class network;
        
    public:
        bool operator==(const event&);
        void set_id(const std::int16_t&);
        void set_name(const std::string&);
        const static event root; // = event(0, "root");
        const static event finish; // = event(-1, "end");

    private:
        std::int16_t _id;
        std::string _name;
        datetime_t _occurence;
        datetime_t _latest_occurence;
        datetime_t _earliest_occurence;
    };

}

pert_cpm::datetime_t operator+(const pert_cpm::datetime_t&, const pert_cpm::time_t&);
pert_cpm::datetime_t operator-(const pert_cpm::datetime_t&, const pert_cpm::time_t&);
pert_cpm::time_t operator-(const pert_cpm::datetime_t&, const pert_cpm::datetime_t&);