from src.checkers.base_checker import BaseChecker
from src.utils import flatten_list
from src.controllers.channels_manager import ChannelsManager
from src.constants import FROZEN

# This class contains the logic to check if channel is frozen (not sending any events through during the
# defined packets_cycle_threshold value in config file)
# channels

class FrozenChecker(BaseChecker):

    """ get the events list from the channel in case it was empty
    we set the flag FROZEN value  of the channel to true else we put
    the FROZEN flag value to false

    Args:
         none

    Returns:
         none
    """
    def run(self):
        for channel in ChannelsManager().get_channels():
            events_list = flatten_list(channel.get_events())
            if events_list:
                channel.update_flag(FROZEN, False)
            else:
                channel.update_flag(FROZEN, True)
