from src.checkers.base_checker import BaseChecker
from src.controllers.channels_manager import ChannelsManager
from src.constants import DROP_RATE_THRESHOLD, HIGH_DROP_RATE


# This class contains the logic to check
# drop rate and to set the drop rate flag of the
# channels

class DropRateChecker(BaseChecker):
    """get the drop rate measure value from the channel
       if its larger than defined DROP_RATE_THRESHOLD in the config file
          then we set the flag for HIGH_DROP_RATE  in the channel value to true
       else we set flag for HIGH_DROP_RATE  in the channel value to false

       Args:
            none

       Returns:
            none
       """
    def run(self):
        for channel in ChannelsManager().get_channels():
            drop_rate = channel.get_measure(self._measure_key)
            if(drop_rate > self._parameters[DROP_RATE_THRESHOLD]):
                channel.update_flag(HIGH_DROP_RATE, True)
            else:
                channel.update_flag(HIGH_DROP_RATE, False)
