from src.checkers.base_checker import BaseChecker
from src.channel_manager import ChannelManager
from src.constants import CHANNEL_FILL_THRESHOLD, HIGH_FILL_TIME


class FillTimeChecker(BaseChecker):
    def run(self):
        for channel in ChannelManager().get_channels():
            if (ChannelManager().calculate_channel_time_to_fill(channel) >
               self._config[CHANNEL_FILL_THRESHOLD]):
                channel.update_flag(HIGH_FILL_TIME, True)
            else:
                channel.update_flag(HIGH_FILL_TIME, False)
