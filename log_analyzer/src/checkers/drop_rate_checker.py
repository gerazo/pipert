from src.checkers.base_checker import BaseChecker
from src.channel_manager import ChannelManager
from src.channel_calc import ChannelCalc
from src.constants import DROP_RATE_THRESHOLD, HIGH_DROP_RATE


class DropRateChecker(BaseChecker):
    def run(self):
        for channel in ChannelManager().get_channels():
            if(ChannelCalc(channel).calculate_drop_rate() >
               self._config[DROP_RATE_THRESHOLD]):
                channel.update_flag(HIGH_DROP_RATE, True)
            else:
                channel.update_flag(HIGH_DROP_RATE, False)
