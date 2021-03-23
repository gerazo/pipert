class BaseChecker(object):
    def __init__(self):
        self._config = None

    def run(self):
        raise NotImplementedError

    def set_config(self, config):
        self._config = config
