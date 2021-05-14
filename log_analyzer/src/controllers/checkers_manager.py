from src.module_getter import ModuleGetter


class CheckersManager(object):
    """Running checkers and control their group behaviour
    """

    @staticmethod
    def run():
        """Runs all the checkers
        """

        for checker in ModuleGetter().get_enabled_checkers():
            checker.run()
