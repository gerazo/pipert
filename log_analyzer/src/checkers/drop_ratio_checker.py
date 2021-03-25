from src.checkers.base_checker import BaseChecker
from src.channel_manager import ChannelManager
from src.constants import DROP_RATIO_THRESHOLD, HIGH_DROP_RATIO


class DropRatioChecker(BaseChecker):
    def run(self):
        for channel in ChannelManager().get_channels():
            if(channel.calculate_drop_ratio() >
               self._config[DROP_RATIO_THRESHOLD]):
                channel.update_flag(HIGH_DROP_RATIO, True)
            else:
                channel.update_flag(HIGH_DROP_RATIO, False)
