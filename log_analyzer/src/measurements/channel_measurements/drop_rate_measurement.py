from src.measurements.channel_measurements.base_channel_measurement\
    import BaseChannelMeasurement
from src.constants import PACKET_DROPPED, EXECTION_TIME


class DropRateMeasurement(BaseChannelMeasurement):
    def _measure(self):
        for channel in self._channel_manager.get_channels():
            drop_rate = self.__calculate_drop_rate(channel)
            channel.\
                add_measure(self.
                            _measurement_key, [self._packet_cycle, drop_rate])
    """ calculate the value of 
    drop_rate = number(dropped events) /number(executed events)
    if there is no executed events nor dropped events it will return drop_rate=-1
    if there is no executed events it will returns drop_rate=1 
    if there is no dropped events it will returns drop_rate=0
    Args:
         channel: the channel which we want to calculate the measurement for  
    Returns:
         drop_rate
    """
    def __calculate_drop_rate(self, channel):
        nr_dropped_event = len(channel.get_event(PACKET_DROPPED))
        nr_executed_event = len(channel.get_event(EXECTION_TIME))
        if (not nr_dropped_event) and (not nr_executed_event):
            return -1

        if not nr_executed_event:
            return 1

        if not nr_dropped_event:
            return 0

        return nr_dropped_event/nr_executed_event
