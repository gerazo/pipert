from src.checkers.base_checker import BaseChecker
from src.channel_manager import ChannelManager
from src.constants import (DROP_RATIO_THRESHOLD, HIGH_DROP_RATIO,
                           PACKET_DROPPED, READ_TIME,Execution_Time,HIGH_EXECUTION_TIME,EXECUTION_TIME_THRESHOLD)


class ExecutionTimeChecker(BaseChecker):
    def run(self):
        for channel in ChannelManager().get_channels():
            if(self.__calculate_exectutinme_time(channel) >self._config[EXECUTION_TIME_THRESHOLD]):
                channel.update_flag(HIGH_EXECUTION_TIME, True)
            else:
                channel.update_flag(HIGH_EXECUTION_TIME, False)



    def __calculate_exectutinme_time(self, channel):
        nr_execution_events= channel.get_event(Execution_Time)
        if(len(nr_execution_events)==0) :
            return -1
        else:
            sumAverage=0
            for event in nr_dropped_events :
                sumAverage+=event.get_avg()
        return sumAverage/len(nr_execution_events)