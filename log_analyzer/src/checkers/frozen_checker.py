from src.checkers.base_checker import BaseChecker
from src.constants import FROZEN
from src.utils import flatten_list


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
    def _check(self, channel):
        events_list = flatten_list(channel.get_events())
        flag_val = len(events_list) == 0
        return flag_val

    def _set_flag_name(self):
        return FROZEN
