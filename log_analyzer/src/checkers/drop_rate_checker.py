from src.checkers.base_checker import BaseChecker
from src.channel_manager import ChannelManager
from src.constants import (
                            DROP_RATE_THRESHOLD, HIGH_DROP_RATE,
                            PACKET_DROPPED, EXECTION_TIME)


class DropRateChecker(BaseChecker):
    def run(self):
        for channel in ChannelManager().get_channels():
            drop_rate = self.__calculate_drop_rate(channel)
            channel.add_measure("Drop Rate", [self._packet_cycle, drop_rate])

            if(drop_rate > self._config[DROP_RATE_THRESHOLD]):
                channel.update_flag(HIGH_DROP_RATE, True)
            else:
                channel.update_flag(HIGH_DROP_RATE, False)

        self._packet_cycle += 1

    def __calculate_drop_rate(self, channel):
        nr_dropped_event = len(channel.get_event(PACKET_DROPPED))
        nr_executed_event = len(channel.get_event(EXECTION_TIME))
        if (not nr_dropped_event) and (not nr_executed_event):
            return -1

        if not nr_executed_event:
            return 1

        if not nr_dropped_event:
            return 0

        return nr_dropped_event/nr_executed_event
