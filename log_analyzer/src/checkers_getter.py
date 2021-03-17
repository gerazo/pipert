from config_reader import ConfigReader


class CheckersGetter(object):
    def __init__(self):
        self.config_reader = ConfigReader()
        self.config_reader.read()

    def get_enabled_checkers(self):
        enabled_checkers = []
        for checker in self.config_reader.read_enabled_checkers():
            enabled_checkers.append(self.__import_checker(checker))

        return enabled_checkers

    def __import_checker(self, checker_name):
        class_name = self.__get_class_name(checker_name)
        mod = __import__("checkers."+checker_name, fromlist=[class_name])
        checker = getattr(mod, class_name)

        return checker()

    def __get_class_name(self, checker_name):
        class_name = checker_name[0].upper()
        i = 1
        while i < len(checker_name):
            if checker_name[i] != "_":
                class_name += checker_name[i]
            else:
                i += 1
                class_name += checker_name[i].upper()
            i += 1

        return class_name
