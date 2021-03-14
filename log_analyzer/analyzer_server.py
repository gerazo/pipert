import socket
import requests
from channel_manager import ChannelManager
from packets_manager import PacketsManager
from frozen_checker import FrozenChecker


class AnalyzerServer(object):

    def __init__(self, ip, port):
        self.__ip = ip
        self.__port = port
        self.__output = ""
#channels history dictionar will hold historical data of last n cycle
#events time and numbers per cycle (Ex : how many drops and execution)
        self.channels_history={}

#events statistics are stored here for each channel's event

        self.channels_statistics={}

#this dictionary with hold the calculated flags for each channal
        self.flags={}

    def __configure_server(self):
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.bind((self.__ip, self.__port))
        return sock

    def updateFlags(self):
        channelsNames= self.channels_history.keys()
        for channel in channelsNames:
            if channel not in self.flags:
                #another flags to be added here
                value={channel: {"Frozen":False}}
                self.flags.update(value)


    def __start_server(self):
        s = self.__configure_server()
        pm = PacketsManager()
        cm = ChannelManager()
        while True:
            data, address = s.recvfrom(512)
            self.__output = data
            pm.add(data)
            cm.add_packet(pm.get_latest_packet())
            FrozenChecker().run()
            requests.post("http://127.0.0.1:5000", json=cm.get_channels_dict())

    def run(self):
        self.__configure_server()
        self.__start_server()

    def get_output(self):
        return self.__output


