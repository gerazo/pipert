import unittest
from unittest.mock import patch, Mock

from src.controllers.checkers_manager import CheckersManager

MODULE_GETTER = "src.controllers.checkers_manager.ModuleGetter"


class TestCheckersManager(unittest.TestCase):
    @patch(MODULE_GETTER)
    def test_when_run_should_run_all_checkers(self, module_getter):
        # Given
        checker1 = Mock()
        checker2 = Mock()
        # When
        module_getter.return_value.get_enabled_checkers.return_value = [checker1, checker2]
        CheckersManager().run()
        # Then
        checker1.run.assert_called()
        checker2.run.assert_called()
