from src.measurements.pipeline_measurements.base_pipeline_measurement import BasePipelineMeasurement
from src.constants import EXECTION_TIME, PACKET_PUSHED, PACKET_RETRIEVED


class TotalExecutionTimeRatioMeasurement(BasePipelineMeasurement):
    """ calculate pipeline_executiontime_ratio=
    pipeline_total_execution_time/pipline_total_time
    which is the ration of pipeline thrust time to total exectuion time of all channels
    Args:
     NONE

    Returns:
    pipeline_executiontime_ratio
     """

    def _measure(self):
        pipline_total_time = self.__get_pipline_thrust_spent_time()
        pipeline_total_execution_time = \
            self.__get_pipeline_maximum_Total_execution_time()
        if(pipline_total_time != -1 and pipline_total_time != -1):
            if(pipline_total_time != 0 and pipline_total_time != 0):
                return pipeline_total_execution_time / pipline_total_time
            else:
                return -1

        else:
            return -1
    """ calculate channels_sum_max_execution_times= sum(maximum_execution_times) of all channels
        if there is no execution times in the channels it will return -1

     Args:
     NONE
     
     Returns:
     channels_sum_max_execution_times
     """
    def __get_pipeline_maximum_Total_execution_time(self):
        channels_maximum_execution_times = \
                    [self.__calculate_max_exectutinme_time(x)
                     for x in self._channel_manager.get_channels()]
        while -1 in channels_maximum_execution_times:
                channels_maximum_execution_times.remove(-1)
        if(len(channels_maximum_execution_times) > 0):
            return sum(channels_maximum_execution_times)
        else:
            return -1

    """ calculate channel sum_max_execution_time= sum(maximum_execution_time) 
    if there is no execution times in the channel it will return -1
    
    Args:
        channel: the channel which we want to calculate the measurement for
        
    Returns:
        sum_max_execution_time
    """
    def __calculate_max_exectutinme_time(self, channel):
        execution_events = channel.get_event(EXECTION_TIME)
        nr_execution_events = len(execution_events)

        if not nr_execution_events:
            return -1

        sum_maximum_executiontime = \
            sum([event.get_max() for event in execution_events])

        return sum_maximum_executiontime

    """ calculate pipeline_thrust_time = maximum_passed_time-minimum_passed_time 
    
    Args:
         NONE
         
    Returns:
         pipeline_thrust_time
    """
    def __get_pipline_thrust_spent_time(self):
        minimum_events_spent_times = \
            [self.__get_minimum_passed_time_event(x)
             for x in self._channel_manager.get_channels()]
        maximum_events_spent_times = \
            [self.__get_maximum_passed_time_event(x)
             for x in self._channel_manager.get_channels()]
        while -1 in minimum_events_spent_times:
            minimum_events_spent_times.remove(-1)
        while -1 in maximum_events_spent_times:
            maximum_events_spent_times.remove(-1)
        if(len(minimum_events_spent_times) > 0 and
                len(maximum_events_spent_times) > 0):
            return max(maximum_events_spent_times) \
                   - min(minimum_events_spent_times)
        else:
            return -2

    """ find the timestamp of the first PACKET_PUSHED event  in the channel and
    return it as  minumumi_passed_time
    if there is no PACKET_PUSHED event in channel's logs it will return -1 
    
    Args:
         channel: the channel which we want to calculate the measurement for
         
    Returns:
         miniumum_passed_time
    """
    def __get_minimum_passed_time_event(self, channel):
        channel_events = channel.get_event(PACKET_PUSHED)
        if len(channel_events) > 0:
                min_event_passed_time = \
                    min([x.get_min() for x in channel_events])
        else:
            min_event_passed_time = -1
        return min_event_passed_time

    """ find the timestamp of the last PACKET_RETRIEVED event  in the channel and
    return it as  maximum_passed_time
    if there is no PACKET_RETRIEVED event in channel's logs it will return -1 
    
    Args:
         channel: the channel which we want to calculate the measurement for
         
    Returns:
         maximum_passed_time
    """
    def __get_maximum_passed_time_event(self, channel):
        channel_events = channel.get_event(PACKET_RETRIEVED)
        if len(channel_events) > 0:
            max_event_passed_time = \
                max([x.get_max() for x in channel_events])
        else:
            max_event_passed_time = -1
        return max_event_passed_time
