from src.checkers.base_checker import BaseChecker
from src.channel_manager import ChannelManager
from src.constants import CHANNEL_FILL_THRESHOLD, HIGH_FILL_TIME, FILL_TIME
from src.utils import calc_avg


class FillTimeChecker(BaseChecker):
    def run(self):
        for channel in ChannelManager().get_channels():
            if (self.calculate_channel_time_to_fill(channel) >
               self._config[CHANNEL_FILL_THRESHOLD]):
                channel.update_flag(HIGH_FILL_TIME, True)
            else:
                channel.update_flag(HIGH_FILL_TIME, False)

    def calculate_channel_time_to_fill(self, channel):
        fill_time_avarage = \
            [x.get_avg() for x in channel.get_event(FILL_TIME)]
        if(len(fill_time_avarage) > 0):
            return calc_avg(fill_time_avarage)
        else:
            return 0
