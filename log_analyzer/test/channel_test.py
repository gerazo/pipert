import unittest
from unittest.mock import patch

from src.channel import Channel
from src.event import Event

POINTS_N_EXTRACT_X_AXIS = "src.channel.reduce_points_n_extract_y_axis"


class TestChannel(unittest.TestCase):
    def test_when_creating_attributes_should_be_set(self):
        # Given & When
        c = Channel("c", [], 12)
        # Then
        self.assertEqual(c.get_name(), "c")
        self.assertEqual(c.get_latest_packet_id(), 12)

    def test_when_add_events_should_be_stored(self):
        # Given
        c = Channel("c", [], 12)
        # When
        event = [1, 2, 3]
        c.add_events(event)
        # Then
        self.assertEqual(c.get_events(), [[], event])

    def test_changing_latest_packet_id(self):
        # Given
        c = Channel("c", [], 12)
        # When
        c.set_latest_packet_id(11)
        # Then
        self.assertEqual(11, c.get_latest_packet_id())

    def test_when_add_measure_n_no_current_measures_should_create_n_append(self):
        # Given
        c = Channel("channel", [], 11)
        measurement_point = (1, 1)
        # When
        c.add_measure("measurement", measurement_point)
        # Then
        measures_val = c.get_measure("measurement")
        self.assertEqual(measures_val, 1)

    @patch(POINTS_N_EXTRACT_X_AXIS)
    def test_when_getting_all_measures_should_return_all(self, rdp):
        # Given
        c = Channel("channel", [], 11)
        rdp.return_value = "ok"
        # When
        c.add_measure("measurement-1", (1, 1))
        c.add_measure("measurement-2", (2, 1))
        c.add_measure("measurement-3", (3, 2))
        # Then
        measures = c.get_all_measures()
        expected_measures = {"measurement-1": "ok", "measurement-2": "ok",
                             "measurement-3": "ok"}
        self.assertEqual(measures, expected_measures)

    def test_when_add_flag_should_crate_it_and_assign_value(self):
        # Given
        c = Channel("c", [], 12)
        # When
        c.add_flag("FLAG", True)
        # Then
        flag_val = c.get_flag("FLAG")
        self.assertEqual(flag_val, True)

    def test_when_get_event_should_return_all_events_of_same_type(self):
        # Given
        e = Event("LOGAA", 1, 2, 3, 4, 5)
        e_2 = Event("LOGAA", 1, 2, 3, 4, 5)
        e_3 = Event("LOGAB", 1, 2, 3, 4, 5)
        c = Channel("c", [e], 12)
        # When
        c.add_events([e_2, e_3])
        # Then
        exp = c.get_event("A")
        self.assertEqual(exp, [e, e_2])
