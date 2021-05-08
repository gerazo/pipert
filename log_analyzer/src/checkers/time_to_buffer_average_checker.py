from src.checkers.base_checker import BaseChecker
from src.constants import HIGH_CHANNEL_TIME_TO_BUFFER, CHANNEL_TIME_TO_BUFFER_AVERAGE_THRESHOLD


# This class contains the logic to check time to buffer of the channels
# (average time spent from times packets released from previous channel to the times to buffer
# of current channel) and set its flag value channels
class TimeToBufferAverageChecker(BaseChecker):
    """ get the time to buffer measure value from the channel
       if its larger than defined CHANNEL_TIME_TO_BUFFER_AVERAGE_THRESHOLD in the config file
          then we set the flag for HIGH_CHANNEL_TIME_TO_BUFFER  in the channel value to true
       else we set flag for HIGH_CHANNEL_TIME_TO_BUFFER  in the channel value to false

       Args: none

       Returns: none
    """
    def _check(self, channel):
        drop_rate = channel.get_measure(self._measure_key)
        flag_val = drop_rate > self._parameters[CHANNEL_TIME_TO_BUFFER_AVERAGE_THRESHOLD]
        return flag_val

    def _set_flag_name(self):
        return HIGH_CHANNEL_TIME_TO_BUFFER
