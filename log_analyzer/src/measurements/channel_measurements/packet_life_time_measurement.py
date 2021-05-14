from src.measurements.channel_measurements.base_channel_measurement import BaseChannelMeasurement
from src.constants import (PACKET_DROPPED, EXECTION_TIME, READ_TIME,
                           PACKET_PUSHED, PACKET_RETRIEVED, FILL_TIME)


class PacketLifeTimeMeasurement(BaseChannelMeasurement):
    """Calculate the value of the spent time of packets in specific channels channel_packet_life_time
    """
    def run(self):
        channels = self._channel_manager.get_channels()
        channels_packet_life_time = [self.__calculate_channel_packet_life_time(c)
                                     for c in channels]

        total_packets_life = sum(channels_packet_life_time)
        for i, channel in enumerate(channels):
            if total_packets_life:
                packet_life_time = channels_packet_life_time[i] / total_packets_life * 100
            else:
                packet_life_time = 0

            channel.add_measure(self._measurement_key,
                                [self._packet_cycle, packet_life_time])

        self._packet_cycle += 1

    def __calculate_channel_packet_life_time(self, channel):
        """Calculate the value of the spent time of packets in specific channels channel_packet_life_time
        which is the sum of the following events :
        fill_time_avg =sum of average time spent between acquiring the Packets and pushing it into the buffers
        read_time_avg= sum of average time spent between retrieving a Packet and releasing it by the retriever
        exec_time_avg= sum of average time spent by a Scheduler thread servicing a Channel callback
        (pushed_time_avg-retrieved_time_avg) time between pushing packet into buffer and retrieving it for execution

        Args:
             channel: the channel which we want to calculate the measurement for

        Returns:
             A real number represents the channel share of the packet's life time
        """

        fill_time_avg = self.__sum_average_of_event(channel, FILL_TIME)
        read_time_avg = self.__sum_average_of_event(channel, READ_TIME)
        exec_time_avg = self.__sum_average_of_event(channel, EXECTION_TIME)
        pushed_time_avg = self.__sum_average_of_event(channel, PACKET_PUSHED)
        retrieved_time_avg = self.__sum_average_of_event(channel, PACKET_RETRIEVED)

        channel_packet_life_time = fill_time_avg + read_time_avg + exec_time_avg + abs(retrieved_time_avg -
                                                                                       pushed_time_avg)

        return channel_packet_life_time

    def _measure(self, channel):
        pass

    @staticmethod
    def __sum_average_of_event(channel, event):
        """ calculate the sum of the average values of specific event of the channel sum_average
        Args:
            channel: the channel which we want to calculate the sum of the average values of specific events
            event : the name of the event we want to calculate sum of the average values for

        Returns:
            An integer or real number represent the sum of averages
        """

        avg = [x.get_avg() for x in channel.get_event(event)]
        return sum(avg)
