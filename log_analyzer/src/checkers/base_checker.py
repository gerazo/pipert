from src.controllers.channels_manager import ChannelsManager


class BaseChecker(object):
    def __init__(self):
        self._parameters = None
        self._measure_key = None
        self.__flag_name = self._set_flag_name()
        self._packet_cycle = 1

    def run(self):
        for channel in ChannelsManager().get_channels():
            channel.add_flag(self.__flag_name, self._check(channel))

    def _set_flag_name(self):
        raise NotImplementedError

    def set_parameters(self, parameters):
        self._parameters = parameters

    def set_measure_key(self, m_key):
        self._measure_key = m_key

    def _check(self, channel):
        raise NotImplementedError
