from src.channel_manager import ChannelManager


class BaseMeasurements(object):
    def __init__(self):
        self._packet_cycle = 1
        self._channel_manager = ChannelManager()
        self._measurement_key = None

    def run(self):
        raise NotImplementedError
    
    def _measure(self):
        raise NotImplementedError

    def set_measurement_key(self, m_key):
        self._measurement_key = m_key

    def get_measurement_key(self):
        return self._measurement_key
