from src.checkers.base_checker import BaseChecker
from src.channel_manager import ChannelManager
from src.constants import CHANNEL_TIME_TO_BUFFER, HIGH_CHANNEL_TIME_TO_BUFFER,\
    CHANNEL_TIME_TO_BUFFER_AVERAGE_THRESHOLD


class TimeToBufferAverageChecker(BaseChecker):
    def run(self):
        for channel in ChannelManager().get_channels():
            if (ChannelManager().calculate_channel_time_to_buffer_average(channel) >
               self._config[CHANNEL_TIME_TO_BUFFER_AVERAGE_THRESHOLD]):
                channel.update_flag(HIGH_CHANNEL_TIME_TO_BUFFER, True)
            else:
                channel.update_flag(HIGH_CHANNEL_TIME_TO_BUFFER, False)
