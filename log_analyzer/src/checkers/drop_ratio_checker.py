from src.checkers.base_checker import BaseChecker
from src.channel_manager import ChannelManager
from src.channel_calc import ChannelCalc
from src.constants import (DROP_RATIO_THRESHOLD, HIGH_DROP_RATIO,
                          PACKET_DROPPED, READ_TIME)


class DropRatioChecker(BaseChecker):
    def run(self):
        for channel in ChannelManager().get_channels():
            drop_ratio = self.__calculate_drop_ratio(channel)
            channel.add_measure("Drop Ratio", [self._packet_cycle, drop_ratio])

            if(drop_ratio > self._config[DROP_RATIO_THRESHOLD]):
                channel.update_flag(HIGH_DROP_RATIO, True)
            else:
                channel.update_flag(HIGH_DROP_RATIO, False)

        self._packet_cycle += 1

    def __calculate_drop_ratio(self, channel):
        nr_dropped_event = len(channel.get_event(PACKET_DROPPED))
        nr_read_event = len(channel.get_event(READ_TIME))

        if (not nr_dropped_event) and (not nr_read_event):
            return -1

        if not nr_read_event:
            return 1

        if not nr_dropped_event:
            return 0

        return nr_dropped_event/nr_read_event
