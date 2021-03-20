from .base_checker import BaseChecker
from channel_manager import ChannelManager
from packets_manager import PacketsManager
from constants import DROP_RATE_THRESHOLD, HIGH_DROP_RATE


class DropRateChecker(BaseChecker):
    def run(self):
        for channel in ChannelManager().get_channels():
            if(channel.calculate_drop_rate() >
               self._config[DROP_RATE_THRESHOLD]):
                channel.update_flag(HIGH_DROP_RATE, True)
            else:
                channel.update_flag(HIGH_DROP_RATE, False)
