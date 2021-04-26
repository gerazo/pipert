from src.measurements.channel_measurements.base_channel_measurement import BaseChannelMeasurement
from src.constants import READ_TIME
from src.utils import calc_avg


class ReadTimeMeasurement(BaseChannelMeasurement):
    def _measure(self):
        for channel in self._channel_manager.get_channels():
            read_time = self.__calculate_channel_time_to_read(channel)
            channel.add_measure(self._measurement_key,
                                [self._packet_cycle, read_time])

    """ calculate the value of packet read time
        read_time_average = avg( read_time_events_values)
    if there is no read time events returns read_time_average=0
    
    Args:
         channel: the channel which we want to calculate the measurement for
         
    Returns:
         read_time_average
    """
    def __calculate_channel_time_to_read(self, channel):
        read_time_avarage = \
            [x.get_avg() for x in channel.get_event(READ_TIME)]
        if(len(read_time_avarage) > 0):
            return calc_avg(read_time_avarage)
        else:
            return 0
