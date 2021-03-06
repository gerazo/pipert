from src.checkers.base_checker import BaseChecker
from src.constants import DROP_RATIO_THRESHOLD, HIGH_DROP_RATIO


class DropRatioChecker(BaseChecker):
    """Gets the drop ratio measure value from the channel
    if its larger than defined DROP_RATIO_THRESHOLD in the config file
    then we set the flag for HIGH_DROP_RATIO  in the channel value to true
    else we set flag for HIGH_DROP_RATIO  in the channel value to false
   """

    def _check(self, channel):
        drop_rate = channel.get_measure(self._measure_key)
        flag_val = drop_rate > self._parameters[DROP_RATIO_THRESHOLD]
        return flag_val

    def _set_flag_name(self):
        return HIGH_DROP_RATIO
