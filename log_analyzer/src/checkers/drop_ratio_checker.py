from src.checkers.base_checker import BaseChecker
from src.channel_manager import ChannelManager
from src.constants import DROP_RATIO_THRESHOLD, HIGH_DROP_RATIO, \
    PACKET_DROPPED, READ_TIME


class DropRatioChecker(BaseChecker):
    def run(self):
        for channel in ChannelManager().get_channels():
            drop_ratio = channel.get_measure(self._measure_key)
            if(drop_ratio > self._parameters[DROP_RATIO_THRESHOLD]):
                channel.update_flag(HIGH_DROP_RATIO, True)
            else:
                channel.update_flag(HIGH_DROP_RATIO, False)
