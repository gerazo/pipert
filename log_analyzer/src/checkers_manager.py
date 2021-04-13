from src.checkers_getter import CheckersGetter


class CheckersManager(object):
    def __init__(self):
        self.__checkers = CheckersGetter().get_enabled_checkers()
    def run(self):
        for checker in self.__checkers:
            checker.run()
