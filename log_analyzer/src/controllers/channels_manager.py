from src.channel import Channel
from src.constants import PACKET_PUSHED


class ChannelsManager(object):
    """Controlling the channels from adding them and collect their data for sending
    also creating the channels' map
    """

    class __ChannelManager(object):
        def __init__(self):
            self.__channels = []
            self.__na_channels = []
            self.__channels_map = []
            self.__should_update_map = False

        def add_packet(self, packet):
            """Adding packet's data from sender and receiver to the channels'
            list and the events to its channel

            Args:
                packet: A packet object
            """

            receiver = packet.get_receiver()
            self.__add_receiver(receiver, packet.get_events(), packet.get_id())

            sender = packet.get_sender()
            pushed_events_count = packet.get_event_count(PACKET_PUSHED)
            self.__add_to_channels_map(receiver, sender, pushed_events_count)

        def get_channels_map(self):
            """Create the channels' map to be sent to the GUI server

            Returns:
                A list of all the channels and a list of tuples to show the order of the channels
                in the pipeline
            """

            channels_names = [c.get_name() for c in self.__channels]
            unique_channels = channels_names + self.__na_channels
            channels_dict = {c: i for i, c in enumerate(unique_channels)}
            connections = [(channels_dict[s], channels_dict[r]) for (s, r) in
                           self.__channels_map]
            return unique_channels, connections

        def should_update_map(self):
            """Determine whether is updated or not, in other words
            whether the analyzer_server should send the map.

            Returns:
                A boolean value
            """

            val = self.__should_update_map
            self.__should_update_map = False

            return val

        def get_channels(self):
            """A getter function for the current list exist in the pipeline

            Returns:
                A list of channels
            """

            return self.__channels

        def get_channels_flags(self):
            """A collector for all the flags in the different channels to be sent

            Returns:
                A list of dictionaries of name of the channel and its flags
            """

            return [{"name": channel.get_name(),
                     "flags": channel.get_flags()}
                    for channel in self.__channels]

        def get_channels_measures(self):
            """A collector for all the measurements in the different channels to be sent

            Returns:
                A list of dictionaries of name of the channel and its measurements
            """

            return [{"name": channel.get_name(),
                     "measures": channel.get_all_measures()}
                    for channel in self.__channels]

        def __add_receiver(self, receiver_channel, events, packet_id):
            """Adding the receiving channel to the channels' with its event list if it is new
            and adding events to an existent channel otherwise

            Args:
                receiver_channel: A string of the receiving channel's name
                events: A list of events
                packet_id: An integer of the packet's id
            """

            for channel in self.__channels:
                if channel.get_name() == receiver_channel:
                    channel.add_events(events)
                    channel.set_latest_packet_id(packet_id)
                    return

            c = Channel(receiver_channel, events, packet_id)
            self.__channels.append(c)

        def __add_to_channels_map(self, receiver, sender, pushed_packet_cnt):
            """Investigating of the sender channel's status whether it is a N/A channel
            or not and store the sender if it is a real sender, and store the receiver
            sender pair in the channels_map list

            Args:
                receiver: A string of the receiving channel's name
                sender: A string of the sending channel's name
                pushed_packet_cnt: An integer of the number of the pushed events in the packet
            """

            # This second condition should be deleted when Sending Bug is fixed
            if not pushed_packet_cnt or sender == "N/A":
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

    __instance = None

    def __new__(cls):
        if ChannelsManager.__instance is None:
            ChannelsManager.__instance = ChannelsManager.__ChannelManager()

        return ChannelsManager.__instance
