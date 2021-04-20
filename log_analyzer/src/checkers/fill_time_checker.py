from src.checkers.base_checker import BaseChecker
from src.channel_manager import ChannelManager
from src.constants import CHANNEL_FILL_THRESHOLD, HIGH_FILL_TIME, FILL_TIME
from src.utils import calc_avg


class FillTimeChecker(BaseChecker):
    def run(self):
        for channel in ChannelManager().get_channels():
            fill_time = channel.get_measure(self._measure_key)
            if (fill_time > self._parameters[CHANNEL_FILL_THRESHOLD]):
                channel.update_flag(HIGH_FILL_TIME, True)
            else:
                channel.update_flag(HIGH_FILL_TIME, False)
