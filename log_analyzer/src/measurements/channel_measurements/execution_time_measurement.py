from src.measurements.channel_measurements.base_channel_measurement import BaseChannelMeasurement
from src.constants import EXECTION_TIME


class ExecutionTimeMeasurement(BaseChannelMeasurement):
    def _measure(self):
        for channel in self._channel_manager.get_channels():
            exec_time = self.__calculate_exectutinme_time(channel)
            channel.add_measure(self._measurement_key, 
                                [self._packet_cycle, exec_time])

    """ calculate the value of 
        execution_time_average = sum( execution_time_events_values) / number(execution_time_events)
    if there is no execution time events returns execution_time_average=-1
    
    Args:
         channel: the channel which we want to calculate the measurement for
         
    Returns:
         execution_time_average
    """
    def __calculate_exectutinme_time(self, channel):
        execution_events = channel.get_event(EXECTION_TIME)
        nr_execution_events = len(execution_events)

        if not nr_execution_events:
            return -1

        sum_average = sum([event.get_avg() for event in execution_events])

        return sum_average/nr_execution_events
