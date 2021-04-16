from src.checkers.base_checker import BaseChecker
from src.channel_manager import ChannelManager
from src.constants import CHANNEL_TIME_TO_BUFFER, HIGH_CHANNEL_TIME_TO_BUFFER,\
    CHANNEL_TIME_TO_BUFFER_AVERAGE_THRESHOLD, PACKET_RETRIEVED, EXECTION_TIME
from src.utils import calc_avg


class TimeToBufferAverageChecker(BaseChecker):
    def run(self):
        for channel in ChannelManager().get_channels():
            if (self.
                    calculate_channel_time_to_buffer_average(channel) >
               self._config[CHANNEL_TIME_TO_BUFFER_AVERAGE_THRESHOLD]):
                channel.update_flag(HIGH_CHANNEL_TIME_TO_BUFFER, True)
            else:
                channel.update_flag(HIGH_CHANNEL_TIME_TO_BUFFER, False)

    def calculate_channel_time_to_buffer_average(self, channel):
        previous_channel_connection = \
            list(filter(
                lambda x:
                x[1] == channel.get_name() and not x[0].startswith(
                    "External"),
                ChannelManager().get_channels_map()))
        if(len(previous_channel_connection) == 0):
            return 0
        else:
            previous_channel_name = previous_channel_connection[0][0]
            privious_chaannel = \
                list(filter
                     (lambda x: x.
                      get_name() == previous_channel_name,
                      ChannelManager().get_channels()))[0]
            previous_channel_packet_events_pushed_average = \
                calc_avg(
                    [x.get_avg()
                     for
                     x in (privious_chaannel.get_event(PACKET_RETRIEVED))])
            previous_channel_packet_events_execution_time_average = \
                calc_avg(
                    [x.get_avg()
                     for x
                     in
                     (privious_chaannel.get_event(
                         EXECTION_TIME))])
            channel_packet_pushed_events = \
                channel.get_event("Packet Pushed")
            channel_packet_pushed_events_avg = \
                calc_avg(
                    [x.get_avg() for x in channel_packet_pushed_events])
            result = \
                channel_packet_pushed_events_avg \
                - previous_channel_packet_events_pushed_average \
                + previous_channel_packet_events_execution_time_average
            if(result < 0):
                return -1
            else:
                return result
