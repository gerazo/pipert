from src.checkers.base_checker import BaseChecker
from src.constants import CHANNEL_FILL_THRESHOLD, HIGH_FILL_TIME


class FillTimeChecker(BaseChecker):
    """Gets the fill time measure value from the channel
    if its larger than defined CHANNEL_FILL_THRESHOLD in the config file
    then we set the flag for HIGH_FILL_TIME  in the channel value to true
    else we set flag for HIGH_FILL_TIME  in the channel value to false
    """

    def _check(self, channel):
        drop_rate = channel.get_measure(self._measure_key)
        flag_val = drop_rate > self._parameters[CHANNEL_FILL_THRESHOLD]
        return flag_val

    def _set_flag_name(self):
        return HIGH_FILL_TIME
