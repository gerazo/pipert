from src.measurements.channel_measurements.base_channel_measurement import BaseChannelMeasurement
from src.constants import FILL_TIME
from src.utils import calc_avg


class FillTimeMeasurement(BaseChannelMeasurement):
    def _measure(self):
        for channel in self._channel_manager.get_channels():
            fill_time = self.__calculate_channel_time_to_fill(channel)
            channel.add_measure(self._measurement_key,
                                [self._packet_cycle, fill_time])

    def __calculate_channel_time_to_fill(self, channel):
        fill_time_avarage = \
            [x.get_avg() for x in channel.get_event(FILL_TIME)]
        if(len(fill_time_avarage) > 0):
            return calc_avg(fill_time_avarage)
        else:
            return 0
