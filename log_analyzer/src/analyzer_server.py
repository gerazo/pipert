import socket
import requests
from time import sleep
from src.controllers.channels_manager import ChannelsManager
from src.controllers.packets_manager import PacketsManager
from src.controllers.checkers_manager import CheckersManager
from src.controllers.measurement_manager import MeasurementManager
from src.config_reader import ConfigReader


class AnalyzerServer(object):
    """Receiving, running controllers and sending data to the flask application

    Attributes:
        ip: A string of the ip in which the server will start on
        port: An integer of the port of the server
    """

    def __init__(self, ip, port):
        self.__ip = ip
        self.__port = port
        self.__channels_manager = ChannelsManager()
        self.__checkers_manager = CheckersManager()
        self.__measurement_manager = MeasurementManager()

    def run(self):
        """ Sleeping for 1 second to give the flaks application chance to start
        and then start the analyzer server
        """
        sleep(1)
        self.__start_server()

    def __configure_server(self):
        """Setting the connection to UDP and binding the socket

        Returns:
            A socket
        """

        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        sock.bind((self.__ip, self.__port))
        return sock

    def __start_server(self):
        """Starting the server for receiving packets, running the checkers and measurements controllers
        and sending the checkers, measurements, and channels' map to the flask application
        """

        server = self.__configure_server()
        packets_manager = PacketsManager()
        packets_cycle_threshold = ConfigReader().get_packets_cycle_threshold()
        flags_counter = 0
        measures_counter = 0
        self.__send_measurements_names()
        while True:
            if flags_counter == packets_cycle_threshold:
                self.__measurement_manager.run()
                self.__checkers_manager.run()
                self.__send_flags_and_channels_map()
                flags_counter = 0
                measures_counter += 1
                if measures_counter == 10:
                    self.__send_measurement()
                    measures_counter = 0

            data, address = server.recvfrom(512)
            packets_manager.add(data)
            self.__channels_manager.add_packet(packets_manager.get_latest_packet())
            flags_counter += 1

    def __send_flags_and_channels_map(self):
        """Sending flags of the checkers and the channel to the flask application
        """

        requests.post("http://127.0.0.1:5000", json={"c_dicts": self.__channels_manager.get_channels_flags()})
        if self.__channels_manager.should_update_map():
            unique_channels, c_map = self.__channels_manager.get_channels_map()
            requests.post("http://127.0.0.1:5000", json={"unique_channels": unique_channels, "channels_map": c_map})

    def __send_measurement(self):
        """Sending measurements to the flask application
        """

        requests.post("http://127.0.0.1:5000", json={"m_dicts": self.__channels_manager.get_channels_measures()})
        pipeline_measures = self.__measurement_manager.get_pipeline_measurements()
        requests.post("http://127.0.0.1:5000/measurements", json={"pipeline_measures": pipeline_measures})

    @staticmethod
    def __send_measurements_names():
        """Sending the name of the measurements for the webpage to store it
        before sending the measurements themselves
        """

        pipeline_measures = ConfigReader().get_enabled_pipeline_measurements()
        channel_measures = ConfigReader().get_enabled_channel_measurements()
        measures_to_send = [m for m in pipeline_measures]
        requests.post("http://127.0.0.1:5000/measurements", json={"pipeline_measures_names": measures_to_send})
        measures_to_send = [m for m in channel_measures]
        requests.post("http://127.0.0.1:5000/measurements", json={"channel_measures_names": measures_to_send})
