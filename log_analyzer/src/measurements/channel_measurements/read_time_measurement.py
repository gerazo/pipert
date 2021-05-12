from src.measurements.channel_measurements.base_channel_measurement import BaseChannelMeasurement
from src.constants import READ_TIME
from src.utils import calc_avg


class ReadTimeMeasurement(BaseChannelMeasurement):

    def _measure(self, channel):
        """ calculate the value of packet read time
            read_time_average = avg( read_time_events_values)

        Args:
             channel: the channel which we want to calculate the measurement for

        Returns:
              An integer or real number as read time measurement,
             if there is no read time events returns read_time_average=0
        """

        read_time_average = [x.get_avg() for x in channel.get_event(READ_TIME)]
        if not read_time_average:
            return 0

        return calc_avg(read_time_average)
