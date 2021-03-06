import struct
from sys import byteorder

from src.event import Event
from src.packet import Packet


class PacketDecoder(object):
    def __init__(self, packet):
        self.__packet = [bytes([b]) for b in packet]

    def decode_packet(self):
        """Decoding the packet coming from the profiler into a packet type

        Returns:
            A packet type instance

        Raises:
            ValueError in case of wrong packet format
        """

        pos = 0
        if self.__check_for_correct_packet(self.__packet[pos:pos+4]):
            pos += 4
            receiver_channel_name, pos = self.__get_keyword(pos)
            sender_channel_name, pos = self.__get_keyword(pos)
            events = []
            correct_packet = self.__check_for_correct_packet(self.__packet[pos:pos+4])
            while not correct_packet and pos < len(self.__packet):
                event, pos = self.__get_event(pos)
                events.append(event)
            return Packet(receiver_channel_name, sender_channel_name, events)
        else:
            raise ValueError

    def __get_keyword(self, pos):
        """Builds a string till the first termination character (\x00)

        Args:
            pos: The current position in the packet to start building from

        Returns:
            A string of a keyword and the position where the process finished
        """

        keyword = b""
        while self.__packet[pos] != b"\x00":
            keyword += self.__packet[pos]
            pos += 1
        return keyword.decode("utf-8"), pos+1

    def __get_event(self, pos):
        """instantiating an event type

        Args:
            pos: The current position in the packet to start parsing from

        Returns:
            An event and the position where the process finished
        """

        event_type, pos = self.__get_keyword(pos)
        log_count, pos = self.__get_int_val(pos)
        time_passed, pos = self.__get_int_val(pos)
        min_val, pos = self.__get_float_val(pos)
        max_val, pos = self.__get_float_val(pos)
        avg_val, pos = self.__get_float_val(pos)

        return Event(event_type, log_count,
                     time_passed, min_val, max_val, avg_val), pos

    def __get_int_val(self, pos):
        """Extracting integer value from the packet and translate the bytes into an integer

        Args:
            pos: The current position in the packet to start parsing from

        Returns:
            An integer value and the position where the process finished as an integer
        """

        ret = b""
        for i in range(0, 4):
            ret += self.__packet[pos + i]
        pos += 4
        ret = int.from_bytes(ret, byteorder="big")
        return ret, pos

    def __get_float_val(self, pos):
        """Extracting float value from the packet and translate the bytes into an float

        Args:
            pos: The current position in the packet to start parsing from

        Returns:
            A float value and the position where the process finished as an integer
        """

        ret = b""
        for i in range(0, 8):
            if byteorder == "little":
                ret += self.__packet[pos+(7-i)]
            else:
                ret += self.__packet[pos+i]
        [ret] = struct.unpack('d', ret)
        pos += 8
        return ret, pos

    @staticmethod
    def __check_for_correct_packet(beginning):
        """Checks if the packet starts in a correct format or not

        Args:
            beginning: List of bytes to check

        Returns:
            A boolean value to indicate the sanity of the packet
        """

        actual = b""
        for byte in beginning:
            actual += byte
        actual = actual.decode("utf-8")
        if actual != "DGRP":
            return False
        return True
