class EndianessConverter(object):
    @staticmethod
    def swap_endians(value):
        leftmost_byte = (value & eval('0x000000FF')) >> 0

        left_middle_byte = (value & eval('0x0000FF00')) >> 8

        right_middle_byte = (value & eval('0x00FF0000')) >> 16

        rightmost_byte = (value & eval('0xFF000000')) >> 24

        leftmost_byte <<= 24
        left_middle_byte <<= 16
        right_middle_byte <<= 8
        rightmost_byte <<= 0

        result = (
            leftmost_byte | left_middle_byte |
            right_middle_byte | rightmost_byte
            )

        return result
