import unittest
from src.channel import Channel
from src.constants import (FROZEN, HIGH_DROP_RATE, HIGH_DROP_RATIO,
                           PACKETS_THRESHOULD, EXECTION_TIME, PACKET_DROPPED,
                           READ_TIME)
from src.event import Event


class TestChannel(unittest.TestCase):
    def test_when_creating_all_flags_should_be_false(self):
        # Given & When
        c = Channel("c", [], -1)
        # Then
        flags = [FROZEN, HIGH_DROP_RATE, HIGH_DROP_RATIO]
        vals = [c.get_flag(flag) for flag in flags]
        self.assertFalse(any(vals))

    def test_when_creating_attributes_should_be_set(self):
        # Given & When
        c = Channel("c", [], 12)
        # Then
        self.assertEqual(c.get_name(), "c")
        self.assertEqual(c.get_latest_packt_id(), 12)
        self.assertEqual(c.get_packet_count(), 1)

    def test_when_add_events_should_increase_the_packet_count(self):
        # Given
        c = Channel("c", [], 12)
        # When
        c.add_events([])
        # Then
        self.assertEqual(c.get_packet_count(), 2)

    def test_when_add_events_more_than_threshold_will_clear_events(self):
        # Given
        c = Channel("c", [], 12)
        # When
        for i in range(PACKETS_THRESHOULD):
            c.add_events([])
        # Then
        events_len = len(c.get_events())
        self.assertEqual(events_len, 1)

    def test_changing_latest_packet_id(self):
        # Given
        c = Channel("c", [], 12)
        # When
        c.set_latest_packet_id(11)
        # Then
        self.assertEqual(11, c.get_latest_packt_id())

    def test_when_updating_non_existing_flag_should_raise_error(self):
        # Given
        c = Channel("c", [], 12)
        # When & Then
        self.assertRaises(ValueError, c.update_flag, "NOT_FLAG", True)

    def test_when_updating_correct_flag_should_update(self):
        # Given
        c = Channel("c", [], 12)
        # When
        c.update_flag(FROZEN, True)
        # Then
        self.assertTrue(c.get_flag(FROZEN))

    def test_get_event(self):
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
