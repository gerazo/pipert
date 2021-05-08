from src.checkers.base_checker import BaseChecker
from src.constants import CHANNEL_FILL_THRESHOLD, HIGH_FILL_TIME
from src.controllers.channels_manager import ChannelsManager


# This class contains the logic to check fill time and to set the fill time flag of the
# channels

class FillTimeChecker(BaseChecker):
    """get the fill time measure value from the channel
    if its larger than defined CHANNEL_FILL_THRESHOLD in the config file
        then we set the flag for HIGH_FILL_TIME  in the channel value to true
    else we set flag for HIGH_FILL_TIME  in the channel value to false

    Args:
         none

    Returns:
         none
    """
    def _check(self, channel):
        drop_rate = channel.get_measure(self._measure_key)
        flag_val = drop_rate > self._parameters[CHANNEL_FILL_THRESHOLD]
        return flag_val

    def _set_flag_name(self):
        return HIGH_FILL_TIME
