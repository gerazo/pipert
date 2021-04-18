import socket
import requests
from src.channel_manager import ChannelManager
from src.packets_manager import PacketsManager
from src.checkers_manager import CheckersManager
from src.config_reader import ConfigReader

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
        packets_cycle_threshold = ConfigReader().get_packets_cycle_threshold()
        flags_counter = 0
        measures_counter = 0
        while True:
            if flags_counter == packets_cycle_threshold:
                checkers_manager.run()
                requests.post("http://127.0.0.1:5000",
                              json={"c_dicts": cm.get_channels_flags()})
                if cm.should_update_map():
                    unique_channels, c_map = cm.get_channels_map()
                    requests.post("http://127.0.0.1:5000",
                                  json={"unique_channels": unique_channels,
                                        "channels_map": c_map})
                flags_counter = 0
                measures_counter += 1
                if measures_counter == 10:
                    requests.post("http://127.0.0.1:5000",
                                  json={"m_dicts": cm.get_channels_measures()})
                    measures_counter = 0

            data, address = s.recvfrom(512)
            self.__output = data
            pm.add(data)
            cm.add_packet(pm.get_latest_packet())
            flags_counter += 1

    def run(self):
        self.__start_server()

    def get_output(self):
        return self.__output
