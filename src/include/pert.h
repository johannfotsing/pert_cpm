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

namespace pert
{

    template<typename EventIDType>
    struct activity_t
    {
        EventIDType trigger_event;
        EventIDType completion_event;
        activity_t(EventIDType a_trigger_event, EventIDType a_completion_event) : trigger_event(a_trigger_event), completion_event(a_completion_event) {}
        bool precedes(const EventIDType& e) const { return completion_event == e; }
        bool follows(const EventIDType& e) const { return trigger_event == e; }
        activity_t<EventIDType> reverse() const { return activity_t<EventIDType>(completion_event, trigger_event); }
    };

    template<typename EventIDType, typename DurationType>
    class network_t
    {
    public:
        using activity = activity_t<EventIDType>;
        using network = network_t<EventIDType, DurationType>;
        using event = EventIDType;
        using duration = DurationType;

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

    template<typename EventIDType>
    bool operator<(const activity_t<EventIDType>& a1, const activity_t<EventIDType>& a2)
    {
        return a1.trigger_event < a2.trigger_event or a1.completion_event < a2.completion_event;
    }

    template<typename EventIDType>
    bool operator==(const activity_t<EventIDType>& a1, const activity_t<EventIDType>& a2)
    {
        return a1.trigger_event == a2.trigger_event and a1.completion_event == a2.completion_event;
    }

    template<typename EventIDType, typename DurationType>
    std::set<activity_t<EventIDType>> network_t<EventIDType, DurationType>::activities() const
    {
        std::set<activity_t<EventIDType>> _activities;
        for (const auto& a: __data)
            _activities.emplace(a.first);

        return _activities;
    }

    template<typename EventIDType, typename DurationType>
    network_t<EventIDType, DurationType>& network_t<EventIDType, DurationType>::add_activity(const activity_t<EventIDType>& an_activity, const network_t<EventIDType, DurationType>::duration& a_duration)
    {
        // no two activities can directly connect two events
        auto search = __data.find(an_activity.reverse());
        if (search != __data.end())
            return *this;

        std::pair<activity_t<EventIDType>, DurationType> pair = std::make_pair(an_activity, a_duration);
        __data.insert(pair);
        return *this;
    }

    template<typename EventIDType, typename DurationType>
    network_t<EventIDType, DurationType>& network_t<EventIDType, DurationType>::add_activity(const EventIDType& a_trigger_event, const EventIDType& a_completion_event, const network_t<EventIDType, DurationType>::duration& a_duration)
    {
        return add_activity(activity_t<EventIDType>(a_trigger_event, a_completion_event), a_duration);
    }

    template<typename EventIDType, typename DurationType>
    network_t<EventIDType, DurationType>& network_t<EventIDType, DurationType>::delete_activity(const activity_t<EventIDType>& an_activity)
    {
        __data.erase(an_activity);
        return *this;
    }

    template<typename EventIDType, typename DurationType>
    network_t<EventIDType, DurationType>& network_t<EventIDType, DurationType>::delete_activity(const EventIDType& a_trigger_event, const EventIDType& a_completion_event)
    {
        return delete_activity(activity_t<EventIDType>(a_trigger_event, a_completion_event));
    }

    template<typename EventIDType, typename DurationType>
    DurationType network_t<EventIDType, DurationType>::estimated_duration(const activity_t<EventIDType>& an_activity) const
    {
        return __data.at(an_activity);
    }

    template<typename EventIDType, typename DurationType>
    void network_t<EventIDType, DurationType>::set_estimated_duration(const activity_t<EventIDType>& an_activity, const network_t<EventIDType, DurationType>::duration& a_duration)
    {
        __data.at(an_activity) = a_duration;
    }

    template<typename EventIDType, typename DurationType>
    std::set<activity_t<EventIDType>> network_t<EventIDType, DurationType>::incoming_activities(const EventIDType& an_event) const
    {
        auto _activities = activities();
        std::set<activity_t<EventIDType>> _incoming_activities;
        std::copy_if(_activities.cbegin(), _activities.cend(), std::inserter(_incoming_activities, _incoming_activities.end()), [an_event](const activity_t<EventIDType>& a){ return a.precedes(an_event); });
        return _incoming_activities;
    }

    template<typename EventIDType, typename DurationType>
    std::set<activity_t<EventIDType>> network_t<EventIDType, DurationType>::outgoing_activities(const EventIDType& an_event) const
    {
        auto _activities = activities();
        std::set<activity_t<EventIDType>> _outgoing_activities;
        std::copy_if(_activities.cbegin(), _activities.cend(), std::inserter(_outgoing_activities, _outgoing_activities.end()), [an_event](const activity_t<EventIDType>& a){ return a.follows(an_event); });
        return _outgoing_activities;
    }

    template<typename EventIDType, typename DurationType>
    bool network_t<EventIDType, DurationType>::is_well_formed() const
    {
        return initial_events().size() == 1 and terminal_events().size() == 1;
    }

    template<typename EventIDType, typename DurationType>
    std::set<EventIDType> network_t<EventIDType, DurationType>::initial_events() const
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
    std::set<EventIDType> network_t<EventIDType, DurationType>::terminal_events() const
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
    std::set<EventIDType> network_t<EventIDType, DurationType>::trigger_events() const
    {
        std::set<EventIDType> _trigger_events;
        for (auto& act: __data)
        {
            _trigger_events.emplace(act.first.trigger_event);
        }
        return _trigger_events;
    }

    template<typename EventIDType, typename DurationType>
    std::set<EventIDType> network_t<EventIDType, DurationType>::completion_events() const
    {
        std::set<EventIDType> _completion_events;
        for (auto& act: __data)
        {
            _completion_events.emplace(act.first.completion_event);
        }
        return _completion_events;
    }

    // SCHEDULE

    template<typename EventIDType, typename DurationType>
    void network_t<EventIDType, DurationType>::schedule(const DurationType& an_initial_time, const DurationType& a_terminal_time)
    {
        __initial_time = an_initial_time;
        __terminal_time = a_terminal_time;
    };

    template<typename EventIDType, typename DurationType>
    DurationType network_t<EventIDType, DurationType>::activity_float(const activity_t<EventIDType>& an_activity) const
    {
        return earliest_occurence(an_activity.completion_event) - earliest_finish(an_activity);
    };

    template<typename EventIDType, typename DurationType>
    DurationType network_t<EventIDType, DurationType>::free_float(const activity_t<EventIDType>& an_activity) const
    {
        return latest_occurence(an_activity.completion_event) - earliest_finish(an_activity);
    };

    template<typename EventIDType, typename DurationType>
    DurationType network_t<EventIDType, DurationType>::interfering_float(const activity_t<EventIDType>& an_activity) const
    {
        return free_float(an_activity) - activity_float(an_activity);
    };

    template<typename EventIDType, typename DurationType>
    DurationType network_t<EventIDType, DurationType>::independent_float(const activity_t<EventIDType>& an_activity) const
    {
        return std::max(0, earliest_occurence(an_activity.completion_event) - latest_occurence(an_activity.trigger_event) - estimated_duration(an_activity));
    };

    template<typename EventIDType, typename DurationType>
    DurationType network_t<EventIDType, DurationType>::earliest_occurence(const EventIDType& an_event) const
    {
        // initial event's occurence time is given by schedule
        EventIDType _initial_event = *initial_events().begin();
        if (an_event == _initial_event) 
            return __initial_time;

        // max earliest finish of incoming activities
        std::set<activity_t<EventIDType>> _incoming_activities = incoming_activities(an_event);
        std::list<DurationType> _earliest_finish_times;
        for (const auto& a: _incoming_activities)
        {
            _earliest_finish_times.push_back(earliest_finish(a));
        }
        return *std::max_element(_earliest_finish_times.begin(), _earliest_finish_times.end());
    };

    template<typename EventIDType, typename DurationType>
    DurationType network_t<EventIDType, DurationType>::earliest_finish(const activity_t<EventIDType>& an_activity) const
    {
        // earliest occurence of trigger + estimated duration
        return earliest_occurence(an_activity.trigger_event) + estimated_duration(an_activity);
    };

    template<typename EventIDType, typename DurationType>
    DurationType network_t<EventIDType, DurationType>::latest_occurence(const EventIDType& an_event) const
    {
        // terminal event's occurence time is given by schedule
        EventIDType _terminal_event = *terminal_events().begin();
        if (an_event == _terminal_event)
         {
            return __terminal_time;
         }

        // min latest start of outgoing activities
        std::set<activity_t<EventIDType>> _outgoing_activities = outgoing_activities(an_event);
        std::list<DurationType> _latest_start_times;
        for (const auto& a: _outgoing_activities)
        {
            _latest_start_times.push_back(latest_start(a));
        }
        return *std::min_element(_latest_start_times.begin(), _latest_start_times.end());
    };

    template<typename EventIDType, typename DurationType>
    DurationType network_t<EventIDType, DurationType>::latest_start(const activity_t<EventIDType>& an_activity) const
    {
        // latest occurence of completion - estimated duration
        return latest_occurence(an_activity.completion_event) - estimated_duration(an_activity);
    };

} // namespace pert
