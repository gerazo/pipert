from src.checkers.base_checker import BaseChecker
from src.constants import (HIGH_EXECUTION_TIME, EXECUTION_TIME_THRESHOLD)


class ExecutionTimeChecker(BaseChecker):
    """ get the execution time measure value from the channel
   if its larger than defined EXECUTION_TIME_THRESHOLD in the config file
  then we set the flag for HIGH_EXECUTION_TIME  in the channel value to true
   else we set flag for HIGH_EXECUTION_TIME  in the channel value to false
   """

    def _check(self, channel):
        drop_rate = channel.get_measure(self._measure_key)
        flag_val = drop_rate > self._parameters[EXECUTION_TIME_THRESHOLD]
        return flag_val

    def _set_flag_name(self):
        return HIGH_EXECUTION_TIME
