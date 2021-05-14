import unittest
from src.event import Event


class TestEvent(unittest.TestCase):
    def test_when_init_should_store_fields(self):
        # Given
        event = Event("    A", 1, 2, 3, 4, 5)
        # When
        e_type = event.get_type()
        e_log_count = event.get_log_count()
        e_passed_time = event.get_passed_time()
        e_min_val = event.get_min()
        e_max_val = event.get_max()
        e_avg_val = event.get_avg()
        # Then
        self.assertEqual(e_type, "A")
        self.assertEqual(e_log_count, 1)
        self.assertEqual(e_passed_time, 2)
        self.assertEqual(e_min_val, 3)
        self.assertEqual(e_max_val, 4)
        self.assertEqual(e_avg_val, 5)

    def test_when_set_packet_id_should_store_id(self):
        # Given
        event = Event("    A", 1, 2, 3, 4, 5)
        # When
        event.set_packet_id(11)
        # Then
        self.assertEqual(event.get_packet_id(), 11)

    def test_when_get_dict_should_return_event_dict(self):
        # Given
        event = Event("    A", 1, 2, 3, 4, 5)
        # When
        event_dict = event.get_dict()
        # Then
        expect_dict = {
             "type": "A",
             "log_count": 1,
             "passed_time": 2,
             "min": 3,
             "max": 4,
             "avg": 5,
        }
        self.assertEqual(expect_dict, event_dict)
