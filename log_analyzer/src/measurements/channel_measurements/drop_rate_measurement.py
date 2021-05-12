from src.measurements.channel_measurements.base_channel_measurement import BaseChannelMeasurement
from src.constants import PACKET_DROPPED, EXECTION_TIME


class DropRateMeasurement(BaseChannelMeasurement):
    """Calculate the drop rate of a channel and assign to it as a measurement
    """

    def _measure(self, channel):
        """ calculate the value of drop_rate which is the
         number(dropped events) / number(executed events)

        Args:
             channel: the channel which we want to calculate the measurement for
        Returns:
             An integer or real number represents the drop_rate,
             if there is no executed events nor dropped events it will return -1
             if there is no executed events it will returns 1
             if there is no dropped events it will returns 0
        """

        nr_dropped_event = len(channel.get_event(PACKET_DROPPED))
        nr_executed_event = len(channel.get_event(EXECTION_TIME))
        if (not nr_dropped_event) and (not nr_executed_event):
            return -1

        if not nr_executed_event:
            return 1

        if not nr_dropped_event:
            return 0

        return nr_dropped_event/nr_executed_event
