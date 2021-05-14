import json


class ConfigReader(object):
    class __ConfigReader(object):
        """Reads the configuration file and provide the information
        """

        def __init__(self):
            self.__data = None
            self.__read()

        def __read(self):
            """Opens and reads the configuration file content
            """
            file = open("config.json")
            data = json.load(file)
            self.__data = data

        def get_enabled_checkers(self):
            """Returns the checkers which have true as value for their enabled field

            Returns:
                A list of the enabled checkers
            """

            checkers = self.__data["checkers"]
            enabled_checkers = []
            for checker in checkers:
                if checker["enabled"]:
                    enabled_checkers.append(checker)

            return enabled_checkers

        def get_enabled_pipeline_measurements(self):
            """Return the measurements in the pipeline's measurement list which have true as value for their enabled
            field

            Returns:
                A list of the enabled pipeline's measurements
            """

            return self.__get_enabled_measurements("pipeline_measurements")

        def get_enabled_channel_measurements(self):
            """Return the measurements in the channel's measurement list which have true as value for their enabled
            field

            Returns:
                A list of the enabled channel's measurements
            """

            return self.__get_enabled_measurements("channel_measurements")

        def __get_enabled_measurements(self, measurements_type):
            """Reads and store the enabled measurement of the type provided as input

            Args:
                measurements_type: A string of the measurement's type

            Returns:
                A list of a tuples of measurements' names and keys
            """

            measurements = self.__data[measurements_type]
            enabled_measurements = []
            for measurement in measurements:
                if measurement["enabled"]:
                    enabled_measurements.append((measurement["name"],
                                                 measurement["key"]))

            return enabled_measurements

        def get_ip_n_port(self):
            """Return the ip and port configured

            Returns:
                A string for the ip and an integer for the port
            """

            ip = self.__data["ip"]
            port = self.__data["port"]

            return ip, port

        def get_packets_cycle_threshold(self):
            """Returns the threshold of the packet's cycle

            Returns:
                An integer of the packet's cycle
            """

            return self.__data["packets_cycle_threshold"]

    __instance = None

    def __new__(cls):
        if ConfigReader.__instance is None:
            ConfigReader.__instance = ConfigReader.__ConfigReader()

        return ConfigReader.__instance
