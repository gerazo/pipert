#include "pipert/Scheduler.h"
#include "pipert/Profiler.h"
#include <thread>
#include <string>
#include <algorithm>
#include<iostream>
#include <unistd.h>
#include <termio.h>
#include <fcntl.h>
#include "longDelayChannelScenario.h"


namespace measurements {


    int main() {
        longDelayChannelScenario long_delay_scenario;
        //press any key while running to stop streaming
        long_delay_scenario.run("UDP:127.0.0.1:888");

        return 0;
    }
}