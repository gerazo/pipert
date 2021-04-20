from src.checkers.base_checker import BaseChecker
from src.channel_manager import ChannelManager
from src.constants import CHANNEL_READ_THRESHOLD, HIGH_READ_TIME, READ_TIME
from src.utils import calc_avg


class ReadTimeChecker(BaseChecker):
    def run(self):
        for channel in ChannelManager().get_channels():
            read_time = channel.get_measure(self._measure_key)
            if (read_time > self._parameters[CHANNEL_READ_THRESHOLD]):
                channel.update_flag(HIGH_READ_TIME, True)
            else:
                channel.update_flag(HIGH_READ_TIME, False)
