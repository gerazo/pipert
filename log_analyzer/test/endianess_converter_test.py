import unittest

from src.endianess_converter import EndianessConverter


class TestEndianessConverter(unittest.TestCase):
    def test_when_big_endian_should_return_little_endian(self):
        # Given & When
        output = EndianessConverter().swap_endianess(eval("0x78563412"))
        # Then
        self.assertEqual(output, eval("0x12345678"))

    def test_when_little_endian_should_return_big_endian(self):
        # Given & When
        output = EndianessConverter().swap_endianess(eval("0x12345678"))
        # Then
        self.assertEqual(output, eval("0x78563412"))
