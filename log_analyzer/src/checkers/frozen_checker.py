from src.checkers.base_checker import BaseChecker
from src.utils import flatten_list
from src.channel_manager import ChannelManager
from src.constants import FROZEN


class FrozenChecker(BaseChecker):
    def run(self):
        for channel in ChannelManager().get_channels():
            events_list = flatten_list(channel.get_events())
            if events_list:
                channel.update_flag(FROZEN, False)
            else:
                channel.update_flag(FROZEN, True)
