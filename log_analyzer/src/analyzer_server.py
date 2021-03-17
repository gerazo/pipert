import socket
import requests
from channel_manager import ChannelManager
from packets_manager import PacketsManager
from checkers_manager import CheckersManager


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
        pm = PacketsManager()
        cm = ChannelManager()
        checkers_manager = CheckersManager()
        while True:
            data, address = s.recvfrom(512)
            self.__output = data
            pm.add(data)
            cm.add_packet(pm.get_latest_packet())
            checkers_manager.run()
            requests.post("http://127.0.0.1:5000", json=cm.get_channels_dict())

    def run(self):
        self.__configure_server()
        self.__start_server()

    def get_output(self):
        return self.__output
