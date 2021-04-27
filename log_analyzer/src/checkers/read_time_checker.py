from src.checkers.base_checker import BaseChecker
from src.channel_manager import ChannelManager
from src.constants import CHANNEL_READ_THRESHOLD, HIGH_READ_TIME, READ_TIME


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
    def run(self):
        for channel in ChannelManager().get_channels():
            read_time = channel.get_measure(self._measure_key)
            if (read_time > self._parameters[CHANNEL_READ_THRESHOLD]):
                channel.update_flag(HIGH_READ_TIME, True)
            else:
                channel.update_flag(HIGH_READ_TIME, False)
