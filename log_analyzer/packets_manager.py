from packet_decoder import PacketDecoder


class PacketsManager(object):
    def __init__(self):
        self.__packets = []
        self.__latest_packet = None

    def add(self, packet):
        packet = PacketDecoder(packet).decode_packet()
        self.__latest_packet = packet
        for p in self.__packets:
            if p.get_receiver() == packet.get_receiver() and p.get_sender() == packet.get_sender():
                p.add_events(packet.get_events())
                return

        self.__packets.append(packet)

    def get_packets(self):
        return self.__packets

    def get_latest_packet(self):
        return self.__latest_packet
