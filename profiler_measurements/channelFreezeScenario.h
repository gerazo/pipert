#ifndef PIPERT_CHANNELFREEZE_SCENARIO_H
#define PIPERT_CHANNELFREEZE_SCENARIO_H



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
    class NumberTocalculate {
    public:
        NumberTocalculate(int number) : number_(number){}
        int Value() const { return number_; }

    private:
        int number_;
    };

    class NumberAbsolute {
    public:
        NumberAbsolute(pipert::PolledChannel<NumberTocalculate> *pc_to_write)
                : pc_to_write_(pc_to_write) {}


        void getNumberAbsolute(pipert::PacketToProcess<NumberTocalculate> p) {

            pc_to_write_->Acquire(p.timestamp() + 10, abs(p.data().Value()) );
        }

    private:
        pipert::PolledChannel<NumberTocalculate> *pc_to_write_;
    };

    class NumberSqrt {
    public:


        void freeze(){
            int timeout=10000000;
            while (timeout>0) {
                timeout--;
            }
        }
        NumberSqrt(pipert::ScheduledChannel<NumberTocalculate> *ch_to_write)
                : ch_to_write_(ch_to_write) {}

        void SqrtNumber(pipert::PacketToProcess<NumberTocalculate> p) {
         if(p.data().Value()>50)
             freeze();
            ch_to_write_->Acquire(p.timestamp() + 5, sqrt(p.data().Value()));
        }

    private:
        pipert::ScheduledChannel<NumberTocalculate> *ch_to_write_;
    };



    class channelFreezeScenario{
    public:
        void run(const char *destination_uri) {
            pipert::Scheduler sch(0, pipert::Profiler(destination_uri));

            int channel_capacity = 100;
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
                            "PrinterChannel", channel_capacity, nullptr,
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
