/***
 * @brief 
 * @author Johann Fotsing
 * @date 2024-09-10
 * @file pert.h
 */

#pragma once

#include <map>
#include <set>
#include <algorithm>
#include <list>
#include <iostream>
#include <vector>
#include <string>
#include <bits/stdc++.h>

namespace pert
{

    //template<typename EventIDType>

    template<typename EventIDType, typename DurationType>
    class network
    {
    public:
        // activity
        class activity
        {
        public:
            activity(EventIDType a_trigger_event, EventIDType a_completion_event) : __trigger_event(a_trigger_event), __completion_event(a_completion_event) {}
            bool precedes(const EventIDType& e) const { return __completion_event == e; }
            bool follows(const EventIDType& e) const { return __trigger_event == e; }
            EventIDType trigger_event() const { return __trigger_event; }
            EventIDType completion_event() const { return __completion_event; }
            activity reverse() const { return activity(__completion_event, __trigger_event); }
            bool operator<(const activity& a) const
            {
                if (__trigger_event == a.__trigger_event)
                    return __completion_event < a.__completion_event;
                
                return __trigger_event < a.__trigger_event;
            };
            bool operator==(const activity& a) const
            {
                
                return __trigger_event == a.__trigger_event and __completion_event == a.__completion_event;
            };
        private:
            EventIDType __trigger_event;
            EventIDType __completion_event;
        };
        // schedule
        struct schedule
        {
            DurationType initial_time;
            DurationType terminal_time;
            schedule(DurationType an_initial_time, DurationType a_terminal_time) : initial_time(an_initial_time), terminal_time(a_terminal_time) {}
        };
        using event = EventIDType;
        using duration = DurationType;
        using segment = std::pair<activity, duration>;
    public:
        std::set<activity> activities() const;
        network& add_activity(const event&, const event&, const duration&);
        network& add_activity(const activity&, const duration&);
        network& delete_activity(const event&, const event&);
        network& delete_activity(const activity&);
        duration estimated_duration(const activity&) const;
        void set_estimated_duration(const activity&, const duration&);
        bool is_well_formed() const;
        std::set<event> initial_events() const;
        std::set<event> terminal_events() const;
        // schedule
        void schedule(const duration&, const duration&);
        duration initial_time() const;
        duration terminal_time() const;
        duration activity_float(const activity&) const;
        duration free_float(const activity&) const;
        duration interfering_float(const activity&) const;
        duration independent_float(const activity&) const;
        // - forward pass
        duration earliest_occurence(const event&) const;
        duration earliest_finish(const activity&) const;
        // - backward pass
        duration latest_occurence(const event&) const;
        duration latest_start(const activity&) const;
        // critical path
        std::vector<segment> find_critical_path() const;
        // automation
        static network from_txt(const std::string &);
        
    private:
        std::set<event> trigger_events() const;
        std::set<event> completion_events() const;
        std::set<activity> incoming_activities(const event&) const;
        std::set<activity> outgoing_activities(const event&) const;

    // data members
    private:
        std::map<activity, duration> __data;
        duration __initial_time;
        duration __terminal_time;
    };

    template<typename EventIDType, typename DurationType>
    bool operator<(const typename network<EventIDType, DurationType>::activity& a1, const typename network<EventIDType, DurationType>::activity& a2)
    {
        return a1 < a2;
    }

    template<typename EventIDType, typename DurationType>
    bool operator==(const typename network<EventIDType, DurationType>::activity& a1, const typename network<EventIDType, DurationType>::activity& a2)
    {
        return a1 == a2;
    }

    template<typename EventIDType, typename DurationType>
    std::set<typename network<EventIDType, DurationType>::activity> network<EventIDType, DurationType>::activities() const
    {
        std::set<typename network<EventIDType, DurationType>::activity> _activities;
        for (const auto& a: __data)
            _activities.emplace(a.first);

        return _activities;
    }

    template<typename EventIDType, typename DurationType>
    network<EventIDType, DurationType>& network<EventIDType, DurationType>::add_activity(const typename network<EventIDType, DurationType>::activity& an_activity, const DurationType& a_duration)
    {
        // no two activities can directly connect two events
        auto search = __data.find(an_activity.reverse());
        if (search != __data.end())
        {
            // DEBUG
            std::cout << "Reverse present: " << an_activity.trigger_event() << " ---> " << an_activity.completion_event() << std::endl;
            return *this;
        }

        __data.insert({an_activity, a_duration});

        return *this;
    }

    template<typename EventIDType, typename DurationType>
    network<EventIDType, DurationType>& network<EventIDType, DurationType>::add_activity(const EventIDType& a_trigger_event, const EventIDType& a_completion_event, const DurationType& a_duration)
    {
        return add_activity(network<EventIDType, DurationType>::activity(a_trigger_event, a_completion_event), a_duration);
    }

    template<typename EventIDType, typename DurationType>
    network<EventIDType, DurationType>& network<EventIDType, DurationType>::delete_activity(const network<EventIDType, DurationType>::activity& an_activity)
    {
        __data.erase(an_activity);
        return *this;
    }

    template<typename EventIDType, typename DurationType>
    network<EventIDType, DurationType>& network<EventIDType, DurationType>::delete_activity(const EventIDType& a_trigger_event, const EventIDType& a_completion_event)
    {
        return delete_activity(network<EventIDType, DurationType>::activity(a_trigger_event, a_completion_event));
    }

    template<typename EventIDType, typename DurationType>
    DurationType network<EventIDType, DurationType>::estimated_duration(const network<EventIDType, DurationType>::activity& an_activity) const
    {
        // DEBUG ?
        auto search_activity = __data.find(an_activity);
        if (search_activity == __data.end())
            return -1;

        return __data.at(an_activity);
    }

    template<typename EventIDType, typename DurationType>
    void network<EventIDType, DurationType>::set_estimated_duration(const network<EventIDType, DurationType>::activity& an_activity, const DurationType& a_duration)
    {
        __data[an_activity] = a_duration;
    }

    template<typename EventIDType, typename DurationType>
    std::set<typename network<EventIDType, DurationType>::activity> network<EventIDType, DurationType>::incoming_activities(const EventIDType& an_event) const
    {
        auto _activities = activities();
        std::set<network<EventIDType, DurationType>::activity> _incoming_activities;
        std::copy_if(_activities.cbegin(), _activities.cend(), std::inserter(_incoming_activities, _incoming_activities.end()), [an_event](const typename network<EventIDType, DurationType>::activity& a){ return a.precedes(an_event); });
        return _incoming_activities;
    }

    template<typename EventIDType, typename DurationType>
    std::set<typename network<EventIDType, DurationType>::activity> network<EventIDType, DurationType>::outgoing_activities(const EventIDType& an_event) const
    {
        auto _activities = activities();
        std::set<typename network<EventIDType, DurationType>::activity> _outgoing_activities;
        std::copy_if(_activities.cbegin(), _activities.cend(), std::inserter(_outgoing_activities, _outgoing_activities.end()), [an_event](const typename network<EventIDType, DurationType>::activity& a){ return a.follows(an_event); });
        return _outgoing_activities;
    }

    template<typename EventIDType, typename DurationType>
    bool network<EventIDType, DurationType>::is_well_formed() const
    {
        return initial_events().size() == 1 and terminal_events().size() == 1;
    }

    template<typename EventIDType, typename DurationType>
    std::set<EventIDType> network<EventIDType, DurationType>::initial_events() const
    {
        std::set<EventIDType> _initial_events;
        auto _trigger_events = trigger_events();
        auto _completion_events = completion_events();
        for (const auto& e: _trigger_events)
        {
            if (_completion_events.find(e) == _completion_events.end())
                _initial_events.emplace(e);
        }
        return _initial_events;
    }

    template<typename EventIDType, typename DurationType>
    std::set<EventIDType> network<EventIDType, DurationType>::terminal_events() const
    {
        std::set<EventIDType> _terminal_events;
        auto _trigger_events = trigger_events();
        auto _completion_events = completion_events();
        for (const auto& e: _completion_events)
        {
            if (_trigger_events.find(e) == _trigger_events.end())
                _terminal_events.emplace(e);
        }
        return _terminal_events;
    }

    template<typename EventIDType, typename DurationType>
    std::set<EventIDType> network<EventIDType, DurationType>::trigger_events() const
    {
        std::set<EventIDType> _trigger_events;
        for (auto& act: __data)
        {
            _trigger_events.emplace(act.first.trigger_event());
        }
        return _trigger_events;
    }

    template<typename EventIDType, typename DurationType>
    std::set<EventIDType> network<EventIDType, DurationType>::completion_events() const
    {
        std::set<EventIDType> _completion_events;
        for (auto& act: __data)
        {
            _completion_events.emplace(act.first.completion_event());
        }
        return _completion_events;
    }

    // SCHEDULE

    template<typename EventIDType, typename DurationType>
    void network<EventIDType, DurationType>::schedule(const DurationType& an_initial_time, const DurationType& a_terminal_time)
    {
        __initial_time = an_initial_time;
        __terminal_time = a_terminal_time;
    };

    template<typename EventIDType, typename DurationType>
    DurationType network<EventIDType, DurationType>::initial_time() const
    {
        return __initial_time;
    }

    template<typename EventIDType, typename DurationType>
    DurationType network<EventIDType, DurationType>::terminal_time() const
    {
        return __terminal_time;
    }

    template<typename EventIDType, typename DurationType>
    DurationType network<EventIDType, DurationType>::activity_float(const network<EventIDType, DurationType>::activity& an_activity) const
    {
        return earliest_occurence(an_activity.completion_event()) - earliest_finish(an_activity);
    };

    template<typename EventIDType, typename DurationType>
    DurationType network<EventIDType, DurationType>::free_float(const network<EventIDType, DurationType>::activity& an_activity) const
    {
        return latest_occurence(an_activity.completion_event()) - earliest_finish(an_activity);
    };

    template<typename EventIDType, typename DurationType>
    DurationType network<EventIDType, DurationType>::interfering_float(const network<EventIDType, DurationType>::activity& an_activity) const
    {
        return free_float(an_activity) - activity_float(an_activity);
    };

    template<typename EventIDType, typename DurationType>
    DurationType network<EventIDType, DurationType>::independent_float(const network<EventIDType, DurationType>::activity& an_activity) const
    {
        return std::max(0, earliest_occurence(an_activity.completion_event()) - latest_occurence(an_activity.trigger_event()) - estimated_duration(an_activity));
    };

    template<typename EventIDType, typename DurationType>
    DurationType network<EventIDType, DurationType>::earliest_occurence(const EventIDType& an_event) const
    {
        // initial event's occurence time is given by schedule
        EventIDType _initial_event = *initial_events().begin();
        if (an_event == _initial_event) 
            return __initial_time;

        // max earliest finish of incoming activities
        std::set<network<EventIDType, DurationType>::activity> _incoming_activities = incoming_activities(an_event);
        std::list<DurationType> _earliest_finish_times;
        for (const auto& a: _incoming_activities)
        {
            _earliest_finish_times.push_back(earliest_finish(a));
        }
        return *std::max_element(_earliest_finish_times.begin(), _earliest_finish_times.end());
    };

    template<typename EventIDType, typename DurationType>
    DurationType network<EventIDType, DurationType>::earliest_finish(const network<EventIDType, DurationType>::activity& an_activity) const
    {
        // earliest occurence of trigger + estimated duration
        return earliest_occurence(an_activity.trigger_event()) + estimated_duration(an_activity);
    };

    template<typename EventIDType, typename DurationType>
    DurationType network<EventIDType, DurationType>::latest_occurence(const EventIDType& an_event) const
    {
        // terminal event's occurence time is given by schedule
        EventIDType _terminal_event = *terminal_events().begin();
        if (an_event == _terminal_event)
         {
            return __terminal_time;
         }

        // min latest start of outgoing activities
        std::set<network<EventIDType, DurationType>::activity> _outgoing_activities = outgoing_activities(an_event);
        std::list<DurationType> _latest_start_times;
        for (const auto& a: _outgoing_activities)
        {
            _latest_start_times.push_back(latest_start(a));
        }
        return *std::min_element(_latest_start_times.begin(), _latest_start_times.end());
    };

    template<typename EventIDType, typename DurationType>
    DurationType network<EventIDType, DurationType>::latest_start(const network<EventIDType, DurationType>::activity& an_activity) const
    {
        // latest occurence of completion - estimated duration
        return latest_occurence(an_activity.completion_event()) - estimated_duration(an_activity);
    };

    template<typename EventIDType, typename DurationType>
    std::vector<typename network<EventIDType, DurationType>::segment> network<EventIDType, DurationType>::find_critical_path() const
    {
        // Copy this network
        network a_network = *this;
        // Tight schedule
        a_network.schedule(0, 1);
        a_network.schedule(0, a_network.earliest_occurence(*terminal_events().begin()));
        // Take activities with no free float
        std::vector<typename network<EventIDType, DurationType>::segment> _critical_path;
        std::copy_if(__data.cbegin(), __data.cend(), std::inserter(_critical_path, _critical_path.end()), [a_network](const auto& segment){ return a_network.free_float(segment.first) == 0; });
        std::sort(_critical_path.begin(), _critical_path.end(), [](const auto& s1, const auto& s2){ return s1.first < s2.first; });
        return _critical_path;
    };

    // AUTOMATION

    template<typename EventIDType, typename DurationType>
    network<EventIDType, DurationType> network<EventIDType, DurationType>::from_txt(const std::string& txt)
    {
        network<EventIDType, DurationType> txt_network;
         
        // Split text into lines and lines into event event duration
        std::string _line;
        char delimiter = '\n';
        std::stringstream txt_stream(txt);
        // Get schedule times
        DurationType _initial_time;
        DurationType _terminal_time;
        std::getline(txt_stream, _line, delimiter);
        std::stringstream(_line) >> _initial_time;
        std::getline(txt_stream, _line, delimiter);
        std::stringstream(_line) >> _terminal_time;
        txt_network.schedule(_initial_time, _terminal_time);
        // Get activities
        while (std::getline(txt_stream, _line, delimiter))
        {
            EventIDType s, f;
            DurationType d;
            std::stringstream _line_stream(_line);
            _line_stream >> s >> f >> d;
            txt_network.add_activity(s, f, d);
        }

        return txt_network;
    };

} // namespace pert