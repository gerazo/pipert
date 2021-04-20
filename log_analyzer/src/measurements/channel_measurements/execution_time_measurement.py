from src.measurements.channel_measurements.base_channel_measurement import BaseChannelMeasurement
from src.constants import EXECTION_TIME


class ExecutionTimeMeasurement(BaseChannelMeasurement):
    def _measure(self):
        for channel in self._channel_manager.get_channels():
            exec_time = self.__calculate_exectutinme_time(channel)
            channel.add_measure(self._measurement_key, 
                                [self._packet_cycle, exec_time])

    def __calculate_exectutinme_time(self, channel):
        execution_events = channel.get_event(EXECTION_TIME)
        nr_execution_events = len(execution_events)

        if not nr_execution_events:
            return -1

        sum_average = sum([event.get_avg() for event in execution_events])

        return sum_average/nr_execution_events
