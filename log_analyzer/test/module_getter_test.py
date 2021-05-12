import unittest
from unittest.mock import patch

from src.module_getter import ModuleGetter


class TestModuleGetter(unittest.TestCase):
    def setUp(self):
        config_reader_patcher = patch("src.module_getter.ConfigReader")
        self.__config_reader = config_reader_patcher.start()

    def test_when_get_enabled_checkers_should_return_list_of_their_instances(self):
        # Given
        checker1 = {"name": "frozen_checker", "parameters": [], "measure_key": ""}
        checker2 = {"name": "drop_rate_checker", "parameters": [], "measure_key": ""}
        checkers = [checker1, checker2]
        self.__config_reader.return_value.get_enabled_checkers.return_value = checkers
        # When
        checkers_instance = ModuleGetter().get_enabled_checkers()
        # Then
        self.assertEqual([type(c).__name__ for c in checkers_instance], ['FrozenChecker', 'DropRateChecker'])
