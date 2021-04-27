from src.measurements.channel_measurements.base_channel_measurement import BaseChannelMeasurement
from src.constants import PACKET_DROPPED, READ_TIME


class DropRatioMeasurement(BaseChannelMeasurement):
    def _measure(self):
        for channel in self._channel_manager.get_channels():
            drop_ratio = self.__calculate_drop_ratio(channel)
            channel.add_measure(self._measurement_key, 
                                [self._packet_cycle, drop_ratio])
    """ calculate the value of 
        drop_ratio = number(dropped events) /number(read events)
    if there is no read events nor dropped events it will return drop_ratio=-1
    if there is no read events it will returns drop_ratio=1 
    if there is no dropped events it will returns drop_ratio=0
    
    Args:
         channel: the channel which we want to calculate the measurement for
         
    Returns:
         drop_ratio
    """
    def __calculate_drop_ratio(self, channel):
        nr_dropped_event = len(channel.get_event(PACKET_DROPPED))
        nr_read_event = len(channel.get_event(READ_TIME))

        if (not nr_dropped_event) and (not nr_read_event):
            return -1

        if not nr_read_event:
            return 1

        if not nr_dropped_event:
            return 0

        return nr_dropped_event/nr_read_event
