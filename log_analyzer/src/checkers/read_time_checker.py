from src.checkers.base_checker import BaseChecker
from src.channel_manager import ChannelManager
from src.constants import CHANNEL_READ_THRESHOLD, HIGH_READ_TIME, READ_TIME
from src.utils import calc_avg


class ReadTimeChecker(BaseChecker):
    def run(self):
        for channel in ChannelManager().get_channels():
            if (self.calculate_channel_time_to_read(channel) >
               self._config[CHANNEL_READ_THRESHOLD]):
                channel.update_flag(HIGH_READ_TIME, True)
            else:
                channel.update_flag(HIGH_READ_TIME, False)

    def calculate_channel_time_to_read(self, channel):
        read_time_avarage = \
            [x.get_avg() for x in channel.get_event(READ_TIME)]
        if(len(read_time_avarage) > 0):
            return calc_avg(read_time_avarage)
        else:
            return 0
