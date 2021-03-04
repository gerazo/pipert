#ifndef PIPERT_HIGHDROPPING_SCENARIO_H
#define PIPERT_HIGHDROPPING_SCENARIO_H
#include <fcntl.h>
#include <termio.h>
#include <unistd.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <thread>
#include <math.h>

#include "pipert/Profiler.h"
#include "pipert/Scheduler.h"
#include "utils.h"

/// Measurement Title : drop rate
/// in this measurement we will generate three channels one of them will
/// have high drop rate
///
///
////Expected analysis output
/// to find the channel with the highst drop rate

namespace measurements {



    class droppingRateScenario {

    public:
    void run(const char *destination_uri) {
    pipert::Scheduler sch(0, pipert::Profiler(destination_uri));

    int channel_capacity = 1;
    pipert::PolledChannel<measurements::NumberTocalculate> pc =
            sch.CreatePolledChannel<measurements::NumberTocalculate>("OutChannel",
                                                                     channel_capacity);

    measurements::NumberAbsolute hr(&pc);
    pipert::ScheduledChannel<measurements::NumberTocalculate> sc2 =
            sch.CreateScheduledChannel<measurements::NumberTocalculate>(
                    "sqrtChannel", channel_capacity, nullptr,
                    std::bind(&measurements::NumberAbsolute::getNumberAbsolute, &hr,
                              std::placeholders::_1));

    measurements::NumberSqrt hp(&sc2);
    pipert::ScheduledChannel<measurements::NumberTocalculate> sc1 =
            sch.CreateScheduledChannel<measurements::NumberTocalculate>(
                    "PrinterChannel", 100, nullptr,
                    std::bind(&measurements::NumberSqrt::SqrtNumber, &hp,
                              std::placeholders::_1));

    sch.Start();
    while (!kbhit()) {
        pipert::Timer::Time time = pipert::Timer::time();
        for (int i = 0; i < 100; i++) {
            pipert::PacketToFill<measurements::NumberTocalculate> packet_to_fill =
                    sc1.Acquire(time,i);
        }

        while (pc.GetQueuedBufferLength() != channel_capacity)
            std::this_thread::yield();
        sch.GetProfiler().GatherNSend();
    }
    sch.Stop();

}
    };
}  // namespace measurements
#endif //PIPERT_CHANNELFREEZE_SCENARIO_H
