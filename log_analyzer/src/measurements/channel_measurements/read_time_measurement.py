from src.measurements.channel_measurements.base_channel_measurement import BaseChannelMeasurement
from src.constants import READ_TIME
from src.utils import calc_avg


class ReadTimeMeasurement(BaseChannelMeasurement):
    def _measure(self):
        for channel in self._channel_manager.get_channels():
            read_time = self.__calculate_channel_time_to_read(channel)
            channel.add_measure(self._measurement_key,
                                [self._packet_cycle, read_time])

    def __calculate_channel_time_to_read(self, channel):
        read_time_avarage = \
            [x.get_avg() for x in channel.get_event(READ_TIME)]
        if(len(read_time_avarage) > 0):
            return calc_avg(read_time_avarage)
        else:
            return 0
