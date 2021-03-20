from .base_checker import BaseChecker
from utils import flatten_list
from channel_manager import ChannelManager
from packets_manager import PacketsManager
from constants import FROZEN


class FrozenChecker(BaseChecker):
    def run(self):
        for channel in ChannelManager().get_channels():
            events_list = flatten_list(channel.get_events())
            if events_list:
                channel.update_flag(FROZEN, False)
            else:
                channel.update_flag(FROZEN, True)
