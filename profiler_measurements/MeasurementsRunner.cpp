#include "utils.h"
#include "channelFreezeScenario.h"
#include "longDelayChannelScenario.h"
#include "highDroppingRateScenario.h"


void generateChannelFreezeScenario( ){
    /// press any key to abort
    measurements::channelFreezeScenario channel_freeze_scenario_;
    channel_freeze_scenario_.run("udp:127.0.0.1:8000");
}

void generateLongDelayScenario( ){
    /// press any key to abort
    measurements::longDelayChannelScenario long_delay_scenario_;
    long_delay_scenario_.run("udp:127.0.0.1:8000");
}

void generateDroppingRateScenario( ){
    /// press any key to abort
    measurements::droppingRateScenario dropping_rate_scenatios ;
    dropping_rate_scenatios.run("udp:127.0.0.1:8000");
}

int main(){
    generateDroppingRateScenario();
}