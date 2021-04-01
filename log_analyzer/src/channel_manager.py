from src.channel import Channel
from src.channel_calc import ChannelCalc


class ChannelManager(object):
    class __ChannelManager(object):
        def __init__(self):
            self.__channels = []
            self.__channelsMap = []

        def add_packet(self, packet):
            should_add_reciever = True
            sender_reciever = [packet.get_sender(), packet.get_receiver()]
            if not (sender_reciever in self.__channelsMap):
                self.__channelsMap.append(sender_reciever)
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
            return result

    __instance = None

    def __new__(cls):
        if ChannelManager.__instance is None:
            ChannelManager.__instance = ChannelManager.__ChannelManager()

        return ChannelManager.__instance
