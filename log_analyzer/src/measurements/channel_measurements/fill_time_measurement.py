from src.measurements.channel_measurements.base_channel_measurement import BaseChannelMeasurement
from src.constants import FILL_TIME
from src.utils import calc_avg


class FillTimeMeasurement(BaseChannelMeasurement):

    def _measure(self, channel):
        """Calculate the value of packet fill time
        fill_time_average = avg( fill_time_events_values)

        Args:
             channel: the channel which we want to calculate the measurement for

        Returns:
             An integer or real number as fill time measurement,
             if there is no fill time events returns fill_time_average=0
        """

        fill_time_average = [x.get_avg() for x in channel.get_event(FILL_TIME)]
        if not fill_time_average:
            return 0

        return calc_avg(fill_time_average)
