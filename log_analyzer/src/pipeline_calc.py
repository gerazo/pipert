from src.channel_manager import ChannelManager
from src.constants import PACKET_DROPPED, EXECTION_TIME, READ_TIME, \
    PACKET_PUSHED, PACKET_RETRIEVED, FILL_TIME


class PiplineCalc():

    def calculate_pipeline_total_execution_time_to_pipeline_time(self):
        pipline_total_time = self.get_pipline_thrust_spent_time()
        pipeline_total_execution_time = \
            self.get_pipeline_maximum_Total_execution_time()
        if(pipline_total_time != -1 and pipline_total_time != -1):
            if(pipline_total_time != 0 and pipline_total_time != 0):
                return pipeline_total_execution_time / pipline_total_time
            else:
                return -1

        else:
            return -1

    def get_pipeline_maximum_Total_execution_time(self):
        channels_maximum_execution_times = \
                    [self.calculate_max_exectutinme_time(x)
                     for x in ChannelManager().get_channels()]
        while -1 in channels_maximum_execution_times:
                channels_maximum_execution_times.remove(-1)
        if(len(channels_maximum_execution_times) > 0):
            return sum(channels_maximum_execution_times)
        else:
            return -1

    def calculate_channel_packet_lifetime(self, channel):
        fill_time_avarage = \
            [x.get_avg() for x in channel.get_event(FILL_TIME)]
        read_time_avarage = \
            [x.get_avg() for x in channel.get_event(READ_TIME)]
        execution_time_avarage = \
            [x.get_avg() for x in channel.get_event(EXECTION_TIME)]
        pushed_time_avarage = \
            [x.get_avg() for x in channel.get_event(PACKET_PUSHED)]
        retrieved_time_avarage = \
            [x.get_avg() for x in channel.get_event(PACKET_RETRIEVED)]
        return sum(fill_time_avarage) + sum(
                read_time_avarage) + sum(
                execution_time_avarage) + abs(
                sum(retrieved_time_avarage) - sum(pushed_time_avarage))

    def calculate_channels_packet_lifetime(self):
        channels_packet_lifetime = [
                [x, self.calculate_channel_packet_lifetime(x)]
                for x in ChannelManager().get_channels()]
        tolat_packets_life = \
            sum([y for [x, y] in channels_packet_lifetime])
        if tolat_packets_life == 0:
            return 0
        else:
            return [[x, y/tolat_packets_life * 100]
                    for [x, y] in channels_packet_lifetime]

    def get_pipline_thrust_spent_time(self):
        minimum_events_spent_times = \
            [self.get_minimum_passed_time_event(x)
             for x in ChannelManager().get_channels()]
        maximum_events_spent_times = \
            [self.get_maximum_passed_time_event(x)
             for x in ChannelManager().get_channels()]
        while -1 in minimum_events_spent_times:
            minimum_events_spent_times.remove(-1)
        while -1 in maximum_events_spent_times:
            maximum_events_spent_times.remove(-1)
        if(len(minimum_events_spent_times) > 0 and
                len(maximum_events_spent_times) > 0):
            return max(maximum_events_spent_times) \
                   - min(minimum_events_spent_times)
        else:
            return -2

    def get_minimum_passed_time_event(self, channel):
        channel_events = channel.get_event(PACKET_PUSHED)
        if len(channel_events) > 0:
                min_event_passed_time = \
                    min([x.get_min() for x in channel_events])
        else:
            min_event_passed_time = -1
        return min_event_passed_time

    def get_maximum_passed_time_event(self, channel):
        channel_events = channel.get_event(PACKET_RETRIEVED)
        if len(channel_events) > 0:
            max_event_passed_time = \
                max([x.get_max() for x in channel_events])
        else:
            max_event_passed_time = -1
        return max_event_passed_time

    def calculate_max_exectutinme_time(self, channel):
        execution_events = channel.get_event(EXECTION_TIME)
        nr_execution_events = len(execution_events)

        if not nr_execution_events:
            return -1

        sum_maximum_executiontime = \
            sum([event.get_max() for event in execution_events])

        return sum_maximum_executiontime
