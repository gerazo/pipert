from src.controllers.channels_manager import ChannelsManager


class BaseChecker(object):
    """Base module for the checker and it contains the main
    logic of checking
    """

    def __init__(self):
        self._parameters = None
        self._measure_key = None
        self.__flag_name = self._set_flag_name()
        self._packet_cycle = 1
        self.__channels_manager = ChannelsManager()

    def run(self):
        """Runs the checker for all the channels and adds the flag
        in the channel
        """

        for channel in self.__channels_manager.get_channels():
            channel.add_flag(self.__flag_name, self._check(channel))

    def _set_flag_name(self):
        """Sets the flag name, it is left for overriding
        for the checkers
        """

        raise NotImplementedError

    def set_parameters(self, parameters):
        """Sets the parameter for the checkers coming from the configuration

        Args:
            parameters: A dictionary of parameter and its value
        """

        self._parameters = parameters

    def set_measure_key(self, m_key):
        """Sets the measure key from the configuration for the
        checker to be able to read its value from the channels

        Args:
            m_key: A string of measurement's key
        """

        self._measure_key = m_key

    def set_manager(self, mgr):
        """For testing purposes, to control the output of
        the manager

        Args:
            mgr: A mock to control emulate the behaviour of the manager
        """

        self.__channels_manager = mgr

    def _check(self, channel):
        """Assigning the value for the flag, it is left to be
        overridden by the checkers themselves

        Args:
            channel: A channel to assingn flag's value to
        """

        raise NotImplementedError
