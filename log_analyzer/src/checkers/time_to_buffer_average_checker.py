from src.checkers.base_checker import BaseChecker
from src.channel_manager import ChannelManager
from src.constants import CHANNEL_TIME_TO_BUFFER, HIGH_CHANNEL_TIME_TO_BUFFER,\
    CHANNEL_TIME_TO_BUFFER_AVERAGE_THRESHOLD, PACKET_RETRIEVED, EXECTION_TIME



class TimeToBufferAverageChecker(BaseChecker):
    def run(self):
        for channel in ChannelManager().get_channels():
            time_to_buffer = channel.get_measure(self._measure_key)
            if (time_to_buffer > self._parameters[CHANNEL_TIME_TO_BUFFER_AVERAGE_THRESHOLD]):
                channel.update_flag(HIGH_CHANNEL_TIME_TO_BUFFER, True)
            else:
                channel.update_flag(HIGH_CHANNEL_TIME_TO_BUFFER, False)
