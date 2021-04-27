from src.module_getter import ModuleGetter


class CheckersManager(object):
    def run(self):
        for checker in ModuleGetter().get_enabled_checkers():
            checker.run()
