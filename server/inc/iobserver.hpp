#ifndef IOBSERVER
#define IOBSERVER
#pragma once
#include <memory>
#include <vector>
#include <iostream>
#include <algorithm>
namespace util 
{
    //------------------------------------
    template<typename S>
    class observer 
    {
        public:
        virtual ~observer() {}
        using subject_t = S;
        virtual void observe(subject_t const &e) = 0;
    };//template<typename S> class observer 

    //------------------------------------

    template<typename S, typename Observer = observer<S>, bool Managed = false>
    class observable 
    {
        public:
        virtual ~observable() { clear(); }
        using subject_t = S;
        using observer_t_nacked = Observer;
        using observer_t = std::weak_ptr<observer_t_nacked>;
        using observer_t_shared = std::shared_ptr<observer_t_nacked>;
        observable &add_observer(observer_t const &o) {
        _observers.push_back(o);
        return (*this);
        }
        observable &add_observer(observer_t_shared &o) {
        observer_t wp = o;
        _observers.push_back(wp);
        return (*this);
        }
        observable &remove_observer(observer_t_shared &o) { return remove_observer(o.get()); }
        observable &remove_observer(observer_t_nacked *o) {
        _observers.erase(std::remove_if(_observers.begin(), _observers.end(), [o](observer_t const &rhs) {
            if (auto spt = rhs.lock())
            return spt.get() == o;
            return false;
        }), _observers.end());
        return (*this);
        }
        observable &operator+=(observer_t const &o) { return add_observer(o); }
        observable &operator+=(observer_t_shared &o) { return add_observer(o); }
        observable &operator-=(observer_t_nacked *o) { return remove_observer(o); }
        observable &operator-=(observer_t_shared &o) { return remove_observer(o); }

        public:
        /**
         * @brief fire an event along the observers chain.
         * @param event_or_subject 
         */
        void emit(subject_t const &event_or_subject) 
        {
            for (auto const &wp : _observers)
                if (auto spt = wp.lock())
                spt->observe(event_or_subject);
        }

        private:
        void clear() {
            if (Managed) 
            {

            }
        }

        private:
        std::vector<observer_t> _observers;
    };
    //------------------------------------

} // namespace util


#endif /* IOBSERVER */
