from src.packet_decoder import PacketDecoder


class PacketsManager(object):
    class __PacketsManager(object):
        """ Managing the packets by using the PacketDecoder to decode them and
        assign ids for them and for their events
        """

        def __init__(self):
            self.__latest_packet = None
            self.__packets_count = 1

        def add(self, packet):
            """Decode a packet from the profiler and store in __latest_packet
            variable

            Args:
                packet: Bytes to be decoded using the  PacketDecoder
            """

            packet = PacketDecoder(packet).decode_packet()
            packet.set_id(self.__packets_count)
            packet.set_id_for_events()
            self.__latest_packet = packet
            self.__packets_count += 1

        def get_latest_packet(self):
            """A getter for the last packet added to the mananger

            Returns:
                A packet object
            """

            return self.__latest_packet

        def get_packet_count(self):
            """A getter for the count of the packets arrived

            Returns:
                An integer represents the count of the packets
            """

            return self.__packets_count - 1

    __instance = None

    def __new__(cls):
        if PacketsManager.__instance is None:
            PacketsManager.__instance = PacketsManager.__PacketsManager()

        return PacketsManager.__instance
