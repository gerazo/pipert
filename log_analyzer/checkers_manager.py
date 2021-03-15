from checkers.frozen_checker import FrozenChecker

CHECKERS = [
    FrozenChecker()
]

class CheckersManager(object):
    def run(self):
        for checker in CHECKERS:
            checker.run()