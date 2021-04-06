from src.channel import Channel
from src.channel_calc import ChannelCalc


class ChannelManager(object):
    class __ChannelManager(object):
        def __init__(self):
            self.__channels = []
            self.__channelsMap = []

        def add_packet(self, packet):
            should_add_reciever = True
            if(packet.get_sender() != "N/A"):
                sender_reciever = [packet.get_sender(), packet.get_receiver()]
                if not (sender_reciever in self.__channelsMap):
                    self.__channelsMap.append(sender_reciever)
            else:
                external_channel_name = "external_" + packet.get_receiver()
                if(not (
                        external_channel_name
                        in [x.get_name() for x in self.__channels])):
                    self.__channels.append(
                        Channel(external_channel_name, [], packet.get_id()))

            for channel in self.__channels:
                if channel.get_name() == packet.get_receiver():
                    channel.add_events(packet.get_events())
                    channel.set_latest_packet_id(packet.get_id())
                    should_add_reciever = False

            if should_add_reciever:
                self.__channels.append(
                    Channel(packet.get_receiver(),
                            packet.get_events(), packet.get_id()))

            should_add_sender = True
            for channel in self.__channels:
                if channel.get_name() == packet.get_sender():
                    should_add_sender = False

            if should_add_sender:
                self.__channels.append(Channel(packet.get_sender(), [], -1))

        def get_channels(self):
            return self.__channels

        def get_channels_dict(self):
            return [ChannelCalc(channel).get_dict() for channel in
                    self.__channels]

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

        def calculate_channel_time_to_read(self, channel):
            channelsMap = self.generate_channels_ordered_map()
            # bug may happen here because of NA connections
            if channel.get_name() in channelsMap:
                channelIndex = channelsMap.index(channel.get_name())
                if channelIndex == 0:
                    return 0
                else:
                    previous_channel = channelsMap[channelIndex-1]
                    channel_events = channel.get_events()[0]
                    previous_channel = list(
                        filter(lambda x: x.get_name() == previous_channel,
                               self.__channels))
                    previous_channel_events =\
                        previous_channel[0].get_events()[0]
                    channel_events_passed_times =\
                        [x.get_passed_time() for x in channel_events]
                    previous_channel_events_passed_times = \
                        [x.get_passed_time() for x in previous_channel_events]
                    if(len(channel_events_passed_times
                           ) > 0 and len(previous_channel_events_passed_times
                                         ) > 0):
                        return\
                            ((sum(channel_events_passed_times
                                  )/len(channel_events_passed_times)
                              )-(sum(previous_channel_events_passed_times
                                     )/len(
                                previous_channel_events_passed_times)))
                    else:
                        return 0
            else:
                return 0

        def generate_channels_ordered_map(self):
            channels_map_copy = self.__channelsMap.copy()
            na_connections = list(filter(lambda x: x[0] == "N/A",
                                         channels_map_copy))
            connections_without_na = list(
                filter(lambda x: x not in na_connections, channels_map_copy))
            filtered_na_list = list(
                filter(lambda x: x[1] not in [item[0]
                                              for item in
                                              connections_without_na],
                       na_connections))
            correct_connections = filtered_na_list+connections_without_na
            result = [correct_connections[0][0], correct_connections[0][1]]
            next_to_find = correct_connections[0][1]
            correct_connections.remove(correct_connections[0])
            while(len(correct_connections) > 0):
                next_connection = list(
                    filter(lambda x: x[1] == next_to_find,
                           correct_connections))
                if(len(next_connection) == 0):
                    break
                result.append(next_connection[0][0])
                next_to_find = next_connection[0][0]
                correct_connections.remove(next_connection[0])
            result.reverse()
            return result

    __instance = None

    def __new__(cls):
        if ChannelManager.__instance is None:
            ChannelManager.__instance = ChannelManager.__ChannelManager()

        return ChannelManager.__instance
