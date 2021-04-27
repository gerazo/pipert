from src.checkers.base_checker import BaseChecker
from src.channel_manager import ChannelManager
from src.constants import DROP_RATIO_THRESHOLD, HIGH_DROP_RATIO, \
    PACKET_DROPPED, READ_TIME


# This class contains the logic to check drop ratio and to set the drop ratio flag of the
# channels

class DropRatioChecker(BaseChecker):
    """get the drop ratio measure value from the channel
   if its larger than defined DROP_RATIO_THRESHOLD in the config file
      then we set the flag for HIGH_DROP_RATIO  in the channel value to true
   else we set flag for HIGH_DROP_RATIO  in the channel value to false

   Args:
        none

   Returns:
        none
   """
    def run(self):
        for channel in ChannelManager().get_channels():
            drop_ratio = channel.get_measure(self._measure_key)
            if(drop_ratio > self._parameters[DROP_RATIO_THRESHOLD]):
                channel.update_flag(HIGH_DROP_RATIO, True)
            else:
                channel.update_flag(HIGH_DROP_RATIO, False)
