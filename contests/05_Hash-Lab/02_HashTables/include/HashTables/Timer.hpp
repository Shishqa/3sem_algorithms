/*============================================================================*/
#ifndef HASHTABLES_RESEARCH_TIMER_HPP
#define HASHTABLES_RESEARCH_TIMER_HPP
/*============================================================================*/
#include <chrono>
/*============================================================================*/
namespace Research {

    using Clock     = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;
    using TimeDelta = Clock::duration;

    /*------------------------------------------------------------------------*/

    class Timer {
    public:

        Timer();

        virtual ~Timer() = default;

        void reset();

        [[nodiscard]]
        TimeDelta elapsed() const;

    protected:

        TimePoint start_time;

    };

}
/*============================================================================*/
#endif //HASHTABLES_RESEARCH_TIMER_HPP
/*============================================================================*/