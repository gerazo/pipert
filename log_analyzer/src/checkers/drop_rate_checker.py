from src.checkers.base_checker import BaseChecker
from src.channel_manager import ChannelManager
from src.constants import DROP_RATE_THRESHOLD, HIGH_DROP_RATE


class DropRateChecker(BaseChecker):
    def run(self):
        for channel in ChannelManager().get_channels():
            drop_rate = channel.get_measure(self._measure_key)
            if(drop_rate > self._parameters[DROP_RATE_THRESHOLD]):
                channel.update_flag(HIGH_DROP_RATE, True)
            else:
                channel.update_flag(HIGH_DROP_RATE, False)
