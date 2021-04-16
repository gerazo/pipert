from src.checkers.base_checker import BaseChecker
from src.channel_manager import ChannelManager
from src.channel_calc import ChannelCalc
from src.constants import (HIGH_EXECUTION_TIME, EXECUTION_TIME_THRESHOLD,
                           EXECTION_TIME)


class ExecutionTimeChecker(BaseChecker):
    def run(self):
        for channel in ChannelManager().get_channels():
            execution_time = self.__calculate_exectutinme_time(channel)
            channel.add_measure("Execution Time", [
                self._packet_cycle, execution_time])

            if (execution_time > self._config[EXECUTION_TIME_THRESHOLD]):
                channel.update_flag(HIGH_EXECUTION_TIME, True)
            else:
                channel.update_flag(HIGH_EXECUTION_TIME, False)

        self._packet_cycle += 1

    def __calculate_exectutinme_time(self, channel):
        execution_events = channel.get_event(EXECTION_TIME)
        nr_execution_events = len(execution_events)

        if not nr_execution_events:
            return -1

        sum_average = sum([event.get_avg() for event in execution_events])

        return sum_average/nr_execution_events
