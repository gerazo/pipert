from src.checkers.base_checker import BaseChecker
from src.channel_manager import ChannelManager
from src.channel_calc import ChannelCalc
from src.constants import (HIGH_EXECUTION_TIME, EXECUTION_TIME_THRESHOLD,
                           EXECTION_TIME)


class ExecutionTimeChecker(BaseChecker):
    def run(self):
        for channel in ChannelManager().get_channels():
            execution_time = channel.get_measure(self._measure_key)
            if (execution_time > self._parameters[EXECUTION_TIME_THRESHOLD]):
                channel.update_flag(HIGH_EXECUTION_TIME, True)
            else:
                channel.update_flag(HIGH_EXECUTION_TIME, False)
