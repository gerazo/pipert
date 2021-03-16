from channel import Channel

class ChannelManager(object):
    class __ChannelManager(object):
        def __init__(self):
            self.__channels = []

        def add_packet(self, packet):
            should_add_reciever = True
            for channel in self.__channels:
                if channel.get_name() == packet.get_receiver():
                    channel.add_events(packet.get_events())
                    channel.set_latest_packet_id(packet.get_id())
                    should_add_reciever = False
            
            if should_add_reciever:
                self.__channels.append(Channel(packet.get_receiver(), packet.get_events(), packet.get_id()))

            should_add_sender = True
            for channel in self.__channels:
                if channel.get_name() == packet.get_sender():
                    should_add_sender = False
            
            if should_add_sender:
                self.__channels.append(Channel(packet.get_sender(), [], -1))

        def get_channels(self):
            return self.__channels

        def get_channels_dict(self):
            return [channel.get_dict() for channel in self.__channels]

    __instance = None

    def __new__(cls):
        if ChannelManager.__instance is None:
            ChannelManager.__instance = ChannelManager.__ChannelManager()

        return ChannelManager.__instance  
