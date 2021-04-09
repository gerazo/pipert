import socket
import requests
from src.channel_manager import ChannelManager
from src.packets_manager import PacketsManager
from src.checkers_manager import CheckersManager
from src.constants import PACKETS_THRESHOULD


class AnalyzerServer(object):

    def __init__(self, ip, port):
        self.__ip = ip
        self.__port = port
        self.__output = ""

    def __configure_server(self):
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        sock.bind((self.__ip, self.__port))
        return sock

    def __start_server(self):
        s = self.__configure_server()
        pm = PacketsManager()
        cm = ChannelManager()
        checkers_manager = CheckersManager()
        counter = 0
        while True:
            if counter == PACKETS_THRESHOULD:
                checkers_manager.run()
                requests.post("http://127.0.0.1:5000",
                              json={"c_dicts": cm.get_channels_dict()})
                if cm.should_update_map():
                    unique_channels, c_map = cm.get_channels_map()
                    requests.post("http://127.0.0.1:5000",
                                  json={"unique_channels": unique_channels,
                                        "channels_map": c_map})
                counter = 0

            data, address = s.recvfrom(512)
            self.__output = data
            pm.add(data)
            cm.add_packet(pm.get_latest_packet())
            counter += 1

    def run(self):
        self.__start_server()

    def get_output(self):
        return self.__output
