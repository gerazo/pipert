from src.measurements.channel_measurements.base_channel_measurement import BaseChannelMeasurement
from src.constants import PACKET_DROPPED, READ_TIME


class DropRatioMeasurement(BaseChannelMeasurement):
    """Calculate the drop ratio of a channel and assign to it as a measurement
    """

    def _measure(self, channel):
        """ calculate the value of
            drop_ratio = number(dropped events) /number(read events)

        Args:
             channel: the channel which we want to calculate the measurement for

        Returns:
             An integer or real number represents the drop_ratio,
             if there is no read events nor dropped events it will return drop_ratio=-1
             if there is no read events it will returns drop_ratio=1
             if there is no dropped events it will returns drop_ratio=0
        """

        nr_dropped_event = len(channel.get_event(PACKET_DROPPED))
        nr_read_event = len(channel.get_event(READ_TIME))

        if (not nr_dropped_event) and (not nr_read_event):
            return -1

        if not nr_read_event:
            return 1

        if not nr_dropped_event:
            return 0

        return nr_dropped_event/nr_read_event
