import json


class ConfigReader(object):
    def __init__(self):
        self.__data = None

    def read(self):
        file = open("config.json")
        data = json.load(file)
        self.__data = data

    def read_enabled_checkers(self):
        checkers = self.__data["checkers"]
        enabled_checkers = []
        for checker in checkers:
            if checker["enabled"]:
                enabled_checkers.append((checker["name"],
                                         checker["configuration"]))

        return enabled_checkers
