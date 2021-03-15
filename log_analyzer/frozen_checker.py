from channel_manager import ChannelManager
from packets_manager import PacketsManager

FROZEN_LIMITS = 20

class FrozenChecker(object):
    def run(self):
        for channel in ChannelManager().get_channels():
            if (channel.get_latest_packt_id() - PacketsManager().get_packet_count() > FROZEN_LIMITS) or channel.get_latest_packt_id() < 0 :
                channel.update_flag("FROZEN", True)
            else:
                 channel.update_flag("FROZEN", False)