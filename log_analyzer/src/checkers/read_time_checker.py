from src.checkers.base_checker import BaseChecker
from src.constants import CHANNEL_READ_THRESHOLD, HIGH_READ_TIME


# This class contains the logic to check read time and to set the read time flag of the
# channels

class ReadTimeChecker(BaseChecker):
    """get the read time measure value from the channel
    if its larger than defined CHANNEL_READ_THRESHOLD in the config file
    then we set the flag for HIGH_READ_TIME  in the channel value to true
    else we set flag for HIGH_READ_TIME  in the channel value to false

    Args:
         none

    Returns:
         none
    """
    def _check(self, channel):
        drop_rate = channel.get_measure(self._measure_key)
        flag_val = drop_rate > self._parameters[CHANNEL_READ_THRESHOLD]
        return flag_val

    def _set_flag_name(self):
        return HIGH_READ_TIME
