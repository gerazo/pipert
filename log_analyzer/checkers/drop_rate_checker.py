from channel_manager import ChannelManager
from packets_manager import PacketsManager

# return the flag if the dropping rate is high
#todo to use thresholds instead of fixed values
class DropRateChecker(object):
    def run(self,threshold=0.5):
        for channel in ChannelManager().get_channels():
            if(channel.drop_rate_calculator()>threshold):
                channel.update_flag("HIGHDROPRATE", True)
            else :
                channel.update_flag("HIGHDROPRATE", False)







