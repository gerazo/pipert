from src.measurements.channel_measurements.base_channel_measurement \
    import BaseChannelMeasurement
from src.constants import PACKET_RETRIEVED, EXECTION_TIME, PACKET_PUSHED
from src.utils import calc_avg


class TimeToBufferAverageMeasurement(BaseChannelMeasurement):
    def _measure(self):
        for channel in self._channel_manager.get_channels():
            time_to_buffer = \
                self.__calculate_channel_time_to_buffer_average(channel)
            channel.\
                add_measure(self._measurement_key, [self._packet_cycle,
                                                    time_to_buffer])
    """ calculate the value of time to buffer average of the channel 
    which represent the time packets spend from the end of the execution in the previous channel 
    till they are buffered in the channel
    time_to_buffer_average= channelPushTimeAverage-previousChannalRetrieveTimeAverage+previousChannelExecutionTimeAverage
    if channel have no previous channel then time_to_buffer_average=0
    Args:
         channel: the channel which we want to calculate the measurement for
         
    Returns:
         time_to_buffer_average
    """
    def __calculate_channel_time_to_buffer_average(self, channel):
        previous_channel_connection = \
            list(filter(
                lambda x:
                x[1] == channel.get_name() and not x[0].startswith(
                    "External"),
                self._channel_manager.get_channels_map()))
        if(len(previous_channel_connection) == 0):
            return 0
        else:
            previous_channel_name = previous_channel_connection[0][0]
            privious_chaannel = \
                list(filter
                     (lambda x: x.
                      get_name() == previous_channel_name,
                      self._channel_manager.get_channels()))[0]
            previous_channel_packet_events_retrieved_average = \
                calc_avg(
                    [(x.get_max()-x.get_min())/2
                     for
                     x in (privious_chaannel.get_event(PACKET_RETRIEVED))])
            previous_channel_packet_events_execution_time_average = \
                calc_avg(
                    [(x.get_max()-x.get_min())/2
                     for x
                     in
                     (privious_chaannel.get_event(
                         EXECTION_TIME))])
            channel_packet_pushed_events = \
                channel.get_event(PACKET_PUSHED)

            channel_packet_pushed_events_avg = \
                calc_avg(
                    [(x.get_max()-x.get_min())/2 for
                     x in channel_packet_pushed_events])
            result = \
                channel_packet_pushed_events_avg \
                - previous_channel_packet_events_retrieved_average \
                + previous_channel_packet_events_execution_time_average
            if(result < 0):
                return -1
            else:
                return result
