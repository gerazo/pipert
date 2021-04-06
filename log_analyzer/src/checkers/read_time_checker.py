from src.checkers.base_checker import BaseChecker
from src.channel_manager import ChannelManager
from src.constants import CHANNEL_READ_THRESHOLD, HIGH_READ_TIME


class ReadTimeChecker(BaseChecker):
    def run(self):
        for channel in ChannelManager().get_channels():
            if (ChannelManager().calculate_channel_time_to_read(channel) >
               self._config[CHANNEL_READ_THRESHOLD]):
                channel.update_flag(HIGH_READ_TIME, True)
            else:
                channel.update_flag(HIGH_READ_TIME, False)
