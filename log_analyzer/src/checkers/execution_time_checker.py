from src.checkers.base_checker import BaseChecker
from src.channel_manager import ChannelManager
from src.channel_calc import ChannelCalc
from src.constants import (HIGH_EXECUTION_TIME, EXECUTION_TIME_THRESHOLD,
                           EXECTION_TIME)

# This class contains the logic to check execution time and to set the execution time flag of the
# channels

class ExecutionTimeChecker(BaseChecker):
    """ get the execution time measure value from the channel
   if its larger than defined EXECUTION_TIME_THRESHOLD in the config file
      then we set the flag for HIGH_EXECUTION_TIME  in the channel value to true
   else we set flag for HIGH_EXECUTION_TIME  in the channel value to false

   Args:
        none

   Returns:
        none
   """

    def run(self):
        for channel in ChannelManager().get_channels():
            execution_time = channel.get_measure(self._measure_key)
            if (execution_time > self._parameters[EXECUTION_TIME_THRESHOLD]):
                channel.update_flag(HIGH_EXECUTION_TIME, True)
            else:
                channel.update_flag(HIGH_EXECUTION_TIME, False)
