from .base_checker import BaseChecker
from channel_manager import ChannelManager
from packets_manager import PacketsManager
from constants import DROP_RATIO_THRESHOLD, HIGH_DROP_RATIO


class DropRatioChecker(BaseChecker):
    def run(self):
        for channel in ChannelManager().get_channels():
            if(channel.drop_ratio_calculator() >
               self._config[DROP_RATIO_THRESHOLD]):
                channel.update_flag(HIGH_DROP_RATIO, True)
