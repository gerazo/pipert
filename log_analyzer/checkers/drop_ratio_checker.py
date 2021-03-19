from channel_manager import ChannelManager
from packets_manager import PacketsManager

# return the flag if the dropping ratio is high
#todo to use threshold instead of fixed values
class DropRatioChecker(object):
    def run(self,threshold=0.75):
        for channel in ChannelManager().get_channels():
            if(channel.drop_ratio_calculator()>threshold):
                channel.update_flag("HIGHDROPRATIO", True)







