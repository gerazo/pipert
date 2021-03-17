from channel_manager import ChannelManager
from packets_manager import PacketsManager

FROZEN_LIMITS = 20


class FrozenChecker(object):
    def run(self):
        for channel in ChannelManager().get_channels():
            flatten_list = self.__flatten_list(channel.get_events())
            if flatten_list:
                channel.update_flag("FROZEN", False)
            else:
                channel.update_flag("FROZEN", True)

    def __flatten_list(self, channels_list):
        return [item for sublist in channels_list for item in sublist]
