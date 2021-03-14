from packet_decoder import PacketDecoder

class PacketsManager(object):
    class __PacketsManager(object):
        def __init__(self):
            self.__latest_packet = None
            self.__packets_count = 0

        def add(self, packet):
            packet = PacketDecoder(packet).decode_packet()
            self.__latest_packet = packet
            self.__packets_count += 1

        def get_latest_packet(self):
            return self.__latest_packet

        def get_packet_count(self):
            return self.__packets_count

    __instance = None

    def __new__(cls):
        if PacketsManager.__instance is None:
            PacketsManager.__instance = PacketsManager.__PacketsManager()
        
        return PacketsManager.__instance
