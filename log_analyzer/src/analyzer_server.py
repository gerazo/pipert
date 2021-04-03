import socket
import requests
from src.channel_manager import ChannelManager
from src.packets_manager import PacketsManager
from src.checkers_manager import CheckersManager


class AnalyzerServer(object):

    def __init__(self, ip, port):
        self.__ip = ip
        self.__port = port
        self.__output = ""
        self.__update_map_controller = 0

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
        while True:
            data, address = s.recvfrom(512)
            self.__output = data
            pm.add(data)
            cm.add_packet(pm.get_latest_packet())
            checkers_manager.run()
            requests.post("http://127.0.0.1:5000", 
                          json={"c_dicts": cm.get_channels_dict()})
            self.__send_channels_map(cm)

    def __send_channels_map(self, cm):
       c_map = cm.generate_channels_ordered_map()
       nr_channels_map = len(c_map)
       if nr_channels_map != self.__update_map_controller:
            self.__update_map_controller = nr_channels_map
            requests.post("http://127.0.0.1:5000", 
                          json={"c_map": c_map})

    def run(self):
        self.__configure_server()
        self.__start_server()

    def get_output(self):
        return self.__output
