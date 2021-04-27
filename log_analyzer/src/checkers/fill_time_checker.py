from src.checkers.base_checker import BaseChecker
from src.channel_manager import ChannelManager
from src.constants import CHANNEL_FILL_THRESHOLD, HIGH_FILL_TIME, FILL_TIME
from src.utils import calc_avg


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
    def run(self):
        for channel in ChannelManager().get_channels():
            fill_time = channel.get_measure(self._measure_key)
            if (fill_time > self._parameters[CHANNEL_FILL_THRESHOLD]):
                channel.update_flag(HIGH_FILL_TIME, True)
            else:
                channel.update_flag(HIGH_FILL_TIME, False)
