from src.channel import Channel
from src.constants import EXECTION_TIME, PACKET_PUSHED,\
    PACKET_RETRIEVED, READ_TIME, FILL_TIME
from src.config_reader import ConfigReader


class ChannelManager(object):
    class __ChannelManager(object):
        def __init__(self):
            self.__channels = []
            self.__na_channels = []
            self.__channels_map = []
            self.__should_update_map = False
            self.__packets_cycle_threshold = ConfigReader().get_packets_cycle_threshold()

        def add_packet(self, packet):
            receiver = packet.get_receiver()
            self.__add_reciever(receiver, packet.get_events(), packet.get_id())

            sender = packet.get_sender()
            pushed_events_count = packet.get_event_count(PACKET_PUSHED)
            self.__add_to_channels_map(receiver, sender, pushed_events_count)

        def __add_reciever(self, receiver_channel, events, packet_id):
            for channel in self.__channels:
                if channel.get_name() == receiver_channel:
                    channel.add_events(events, self.__packets_cycle_threshold)
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
                    "measures": channel.get_all_measures()}
                    for channel in self.__channels]
    __instance = None

    def __new__(cls):
        if ChannelManager.__instance is None:
            ChannelManager.__instance = ChannelManager.__ChannelManager()

        return ChannelManager.__instance
