from utils import flatten_list
from channel_manager import ChannelManager
from packets_manager import PacketsManager
from constants import FROZEN_LIMITS, FROZEN

class FrozenChecker(object):
    def run(self):
        for channel in ChannelManager().get_channels():
            flatten_list = flatten_list(channel.get_events())
            if flatten_list:
                channel.update_flag(FROZEN, False)
            else:
                channel.update_flag(FROZEN, True)
