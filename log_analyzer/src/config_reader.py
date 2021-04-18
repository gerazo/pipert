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

        def read_enabled_checkers(self):
            checkers = self.__data["checkers"]
            enabled_checkers = []
            for checker in checkers:
                if checker["enabled"]:
                    enabled_checkers.append((checker["name"],
                                            checker["parameters"]))

            return enabled_checkers

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
