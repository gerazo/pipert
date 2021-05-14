from src.measurements.channel_measurements.base_channel_measurement import BaseChannelMeasurement
from src.constants import EXECTION_TIME


class ExecutionTimeMeasurement(BaseChannelMeasurement):
    """Calculate the execution time of a channel and assign to it as a measurement
    """

    def _measure(self, channel):
        """ calculate the value of
        execution_time_average = sum( execution_time_events_values) / number(execution_time_events)

        Args:
             channel: the channel which we want to calculate the measurement for

        Returns:
             An integer or real number represents the execution time,
             if there is no execution time events returns execution_time_average = -1
        """

        execution_events = channel.get_event(EXECTION_TIME)
        nr_execution_events = len(execution_events)

        if not nr_execution_events:
            return -1

        sum_average = sum([event.get_avg() for event in execution_events])

        return sum_average/nr_execution_events
