import socket

from packet_decoder import PacketDecoder


class AnalyzerServer(object):
    def __init__(self, ip, port):
        self.__ip = ip
        self.__port = port
        self.__output = ""

    def __configure_server(self):
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.bind((self.__ip, self.__port))
        return sock

    def __start_server(self):
        s = self.__configure_server()
        while True:
            data, address = s.recvfrom(4096)
            self.__output = data
            print(PacketDecoder(self.__output).decode_packet(0))

    def run(self):
        self.__configure_server()
        self.__start_server()

    def get_output(self):
        return self.__output
