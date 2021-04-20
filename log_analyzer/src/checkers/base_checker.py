class BaseChecker(object):
    def __init__(self):
        self._parameters = None
        self._measure_key = None
        self._packet_cycle = 1

    def run(self):
        raise NotImplementedError

    def set_parameters(self, parameters):
        self._parameters = parameters

    def set_measure_key(self, m_key):
        self._measure_key = m_key