class BaseChecker(object):
    def __init__(self):
        self._config = None
        self._packet_cycle = 1

    def run(self):
        raise NotImplementedError

    def set_config(self, config):
        self._config = config
