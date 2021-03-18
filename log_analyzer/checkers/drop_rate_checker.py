from channel_manager import ChannelManager
from packets_manager import PacketsManager

FROZEN_LIMITS = 20



# return the flag if the dropping rate is high
# it will return -1 if the channel if both number of executed and dropped events are 0
# if there is no execution only dropping it will return 1
class DropRateChecker(object):
    def run(self,threshold=0.5):
        for channel in ChannelManager().get_channels():
            if(channel.drop_rate_calculator()>threshold):
                channel.update_flag("HIGHDROPRATE", True)







