from src.channel import Channel
from src.channel_calc import ChannelCalc
from src.utils import calc_avg
from src.constants import EXECTION_TIME, PACKET_PUSHED,\
    PACKET_RETRIEVED, READ_TIME, FILL_TIME


class ChannelManager(object):
    class __ChannelManager(object):
        def __init__(self):
            self.__channels = []
            self.__na_channels = []
            self.__channels_map = []
            self.__should_update_map = False

        def add_packet(self, packet):
            receiver = packet.get_receiver()
            self.__add_reciever(receiver, packet.get_events(), packet.get_id())

            sender = packet.get_sender()
            pushed_events_count = packet.get_event_count(PACKET_PUSHED)
            self.__add_to_channels_map(receiver, sender, pushed_events_count)

        def __add_reciever(self, receiver_channel, events, packet_id):
            for channel in self.__channels:
                if channel.get_name() == receiver_channel:
                    channel.add_events(events)
                    channel.set_latest_packet_id(packet_id)
                    should_add_reciever = False
                    return

            c = Channel(receiver_channel, events, packet_id)
            self.__channels.append(c)

        def __add_to_channels_map(self, receiver, sender, cnt):
            # This second condition should be deleted when Sending Bug is fixed
            if not cnt or sender == "N/A":
                return
            if sender == "N/A":
                sender = "External_" + receiver
                if sender not in self.__na_channels:
                    self.__na_channels.append(sender)
            else:
                channel_names = [c.get_name() for c in self.__channels]
                if sender not in channel_names:
                    c = Channel(sender, [], -1)
                    self.__channels.append(c)
            sender_n_receiver = (sender, receiver)
            if sender_n_receiver not in self.__channels_map:
                self.__channels_map.append(sender_n_receiver)
                self.__should_update_map = True

        def get_channels_map(self):
            channels_names = [c.get_name() for c in self.__channels]
            unique_channels = channels_names + self.__na_channels
            channels_dict = {c: i for i, c in enumerate(unique_channels)}
            connections = [(channels_dict[s], channels_dict[r]) for (s, r) in
                           self.__channels_map]
            return unique_channels, connections

        def should_update_map(self):
            val = self.__should_update_map
            self.__should_update_map = False

            return val

        def get_channels(self):
            return self.__channels

        def get_channels_flags(self):
            return [{"name": channel.get_name(),
                    "flags": channel.get_flags()} 
                    for channel in self.__channels]

        def get_channels_measures(self):
            return [{"name": channel.get_name(),
                    "measures": channel.get_measures()}
                    for channel in self.__channels]

        def get_pipline_thrust_spent_time(self):
            minimum_events_spent_times = \
                [ChannelCalc(x).get_minimum_passed_time_event()
                 for x in self.__channels]
            maximum_events_spent_times = \
                [ChannelCalc(x).get_maximum_passed_time_event()
                 for x in self.__channels]
            while -1 in minimum_events_spent_times:
                minimum_events_spent_times.remove(-1)
            while -1 in maximum_events_spent_times:
                maximum_events_spent_times.remove(-1)
            if(len(minimum_events_spent_times) > 0 and
                    len(maximum_events_spent_times) > 0):
                return max(maximum_events_spent_times)\
                       - min(minimum_events_spent_times)
            else:
                return -2

        def get_pipeline_maximum_Total_execution_time(self):
            channels_maximum_execution_times = \
                [ChannelCalc(x).calculate_maximum_exectutinme_time()
                 for x in self.__channels]
            while -1 in channels_maximum_execution_times:
                channels_maximum_execution_times.remove(-1)
            if(len(channels_maximum_execution_times) > 0):
                return sum(channels_maximum_execution_times)
            else:
                return -1

        def calculate_channel_time_to_buffer_average(self, channel):
            previous_channel_connection = \
                list(filter(
                    lambda x:
                    x[1] == channel.get_name() and not x[0].startswith(
                        "External"),
                    self.__channels_map))
            if(len(previous_channel_connection) == 0):
                return 0
            else:
                previous_channel_name = previous_channel_connection[0][0]
                privious_chaannel = \
                    list(filter
                         (lambda x: x.
                          get_name() == previous_channel_name,
                          self.__channels))[0]
                previous_channel_packet_events_pushed_average =\
                    calc_avg(
                        [x.get_avg()
                         for
                         x in (privious_chaannel.get_event(PACKET_RETRIEVED))])
                previous_channel_packet_events_execution_time_average =\
                    calc_avg(
                        [x.get_avg()
                         for x
                         in
                         (privious_chaannel.get_event(
                             EXECTION_TIME))])
                channel_packet_pushed_events =\
                    channel.get_event("Packet Pushed")
                channel_packet_pushed_events_avg =\
                    calc_avg(
                        [x.get_avg() for x in channel_packet_pushed_events])
                result = \
                    channel_packet_pushed_events_avg\
                    - previous_channel_packet_events_pushed_average\
                    + previous_channel_packet_events_execution_time_average
                if(result < 0):
                    return -1
                else:
                    return result

        def calculate_channel_time_to_read(self, channel):
            read_time_avarage =\
                [x.get_avg() for x in channel.get_event(READ_TIME)]
            if(len(read_time_avarage) > 0):
                return calc_avg(read_time_avarage)
            else:
                return 0

        def calculate_channel_time_to_fill(self, channel):
            fill_time_avarage =\
                [x.get_avg() for x in channel.get_event(FILL_TIME)]
            if(len(fill_time_avarage) > 0):
                return calc_avg(fill_time_avarage)
            else:
                return 0

        def calculate_channel_packet_lifetime(self, channel):
            fill_time_avarage =\
                [x.get_avg() for x in channel.get_event(FILL_TIME)]
            read_time_avarage =\
                [x.get_avg() for x in channel.get_event(READ_TIME)]
            execution_time_avarage =\
                [x.get_avg() for x in channel.get_event(EXECTION_TIME)]
            pushed_time_avarage =\
                [x.get_avg() for x in channel.get_event(PACKET_PUSHED)]
            retrieved_time_avarage =\
                [x.get_avg() for x in channel.get_event(PACKET_RETRIEVED)]
            return sum(fill_time_avarage) + sum(
                read_time_avarage) + sum(
                execution_time_avarage) + abs(
                sum(retrieved_time_avarage) - sum(pushed_time_avarage))

        def calculate_channels_packet_lifetime(self):
            channels_packet_lifetime = [
                [x, self.calculate_channel_packet_lifetime(x)]
                for x in self.__channels]
            tolat_packets_life = sum([y for [x, y] in
                                      channels_packet_lifetime])
            if tolat_packets_life == 0:
                return 0
            else:
                return [[x, y/tolat_packets_life * 100]
                        for [x, y] in channels_packet_lifetime]

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

    __instance = None

    def __new__(cls):
        if ChannelManager.__instance is None:
            ChannelManager.__instance = ChannelManager.__ChannelManager()

        return ChannelManager.__instance
