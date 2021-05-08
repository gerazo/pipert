import socket
import requests
from time import sleep
from src.controllers.channels_manager import ChannelsManager
from src.controllers.packets_manager import PacketsManager
from src.controllers.checkers_manager import CheckersManager
from src.controllers.measurement_manager import MeasurementManager
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
        cm = ChannelsManager()
        checkers_manager = CheckersManager()
        measurement_manager = MeasurementManager()
        packets_cycle_threshold = ConfigReader().get_packets_cycle_threshold()
        flags_counter = 0
        measures_counter = 0
        sleep(1)
        self.__send_measurements_names()
        while True:
            if flags_counter == packets_cycle_threshold:
                measurement_manager.run()
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
                    pipeline_measures = measurement_manager.get_pipeline_measurements()
                    requests.post("http://127.0.0.1:5000/measurements",
                                  json={"pipeline_measures": pipeline_measures})
                    measures_counter = 0

            data, address = s.recvfrom(512)
            self.__output = data
            pm.add(data)
            cm.add_packet(pm.get_latest_packet())
            flags_counter += 1

    @staticmethod
    def __send_measurements_names():
        pipeline_measures = ConfigReader().get_enabled_pipeline_measurements()
        channel_measures = ConfigReader().get_enabled_channel_measurements()
        measures_to_send = [m for m in pipeline_measures]
        requests.post("http://127.0.0.1:5000/measurements",
                      json={"pipeline_measures_names": measures_to_send})
        measures_to_send = [m for m in channel_measures]
        requests.post("http://127.0.0.1:5000/measurements",
                      json={"channel_measures_names": measures_to_send})

    def run(self):
        self.__start_server()

    def get_output(self):
        return self.__output
