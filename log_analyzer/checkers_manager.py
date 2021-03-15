from checkers_getter import CheckersGetter

class CheckersManager(object):
    def run(self):
        for checker in CheckersGetter().get_enabled_checkers():
            checker.run()
