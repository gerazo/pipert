import json


class ConfigReader(object):
    class __ConfigReader(object):
        def __init__(self):
            self.__data = None
            self.__read()

        def __read(self):
            file = open("config.json")
            data = json.load(file)
            self.__data = data

        def get_enabled_checkers(self):
            checkers = self.__data["checkers"]
            enabled_checkers = []
            for checker in checkers:
                if checker["enabled"]:
                    enabled_checkers.append(checker)

            return enabled_checkers

        def get_enabled_pipeline_measurements(self):
            return self.__get_enabled_measurements("pipeline_measurements")

        def get_enabled_channel_measurements(self):
            return self.__get_enabled_measurements("channel_measurements")

        def __get_enabled_measurements(self, measurements_type):
            measurements = self.__data[measurements_type]
            enabled_measurements = []
            for measurement in measurements:
                if measurement["enabled"]:
                    enabled_measurements.append((measurement["name"],
                                                 measurement["key"]))

            return enabled_measurements

        def get_ip_n_port(self):
            ip = self.__data["ip"]
            port = self.__data["port"]

            return ip, port

        def get_packets_cycle_threshold(self):
            return self.__data["packets_cycle_threshold"]

    __instance = None

    def __new__(cls):
        if ConfigReader.__instance is None:
            ConfigReader.__instance = ConfigReader.__ConfigReader()

        return ConfigReader.__instance
