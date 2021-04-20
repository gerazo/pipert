from src.measurements.channel_measurements.base_channel_measurement import BaseChannelMeasurement
from src.constants import (PACKET_DROPPED, EXECTION_TIME, READ_TIME,
                          PACKET_PUSHED, PACKET_RETRIEVED, FILL_TIME)


class PacketLifeTimeMeasurement(BaseChannelMeasurement):
    def _measure(self):
        channels = self._channel_manager.get_channels()
        channels_packet_life_time = [self.__calculate_channel_packet_life_time(c)
                                    for c in channels]

        total_packets_life = sum(channels_packet_life_time)
        for i, channel in enumerate(channels):
            packet_life_time =  channels_packet_life_time[i] / total_packets_life * 100
            channel.add_measure(self._measurement_key, 
                                [self._packet_cycle, packet_life_time])
    
    def __calculate_channel_packet_life_time(self, channel):
        fill_time_avg = self.__sum_average_of_event(channel, FILL_TIME)
        read_time_avg = self.__sum_average_of_event(channel, READ_TIME)
        exec_time_avg = self.__sum_average_of_event(channel, EXECTION_TIME)
        pushed_time_avg = self.__sum_average_of_event(channel, PACKET_PUSHED)
        retrieved_time_avg = self.__sum_average_of_event(channel, PACKET_RETRIEVED)

        channel_packet_life_time = fill_time_avg + read_time_avg\
                                   + exec_time_avg + pushed_time_avg\
                                   + retrieved_time_avg
    
        return channel_packet_life_time

    def __sum_average_of_event(self, channel, event):
        avgs = [x.get_avg() for x in channel.get_event(event)]

        return sum(avgs)
