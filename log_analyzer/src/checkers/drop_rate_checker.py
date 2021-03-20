from channel_manager import ChannelManager
from packets_manager import PacketsManager
from constants import DROP_RATE_THRESHOLD, HIGH_DROP_RATE

class DropRateChecker(object):
    def run(self):
        for channel in ChannelManager().get_channels():
            if(channel.drop_rate_calculator()>DROP_RATE_THRESHOLD):
                channel.update_flag(HIGH_DROP_RATE, True)
