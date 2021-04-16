from src.constants import PACKET_DROPPED, EXECTION_TIME, READ_TIME,PACKET_PUSHED,PACKET_RETRIEVED


class ChannelCalc(object):
    def __init__(self, channel):
        self.__channel = channel

    def get_minimum_passed_time_event(self):
        channel_events = self.__channel.get_event(PACKET_PUSHED)
        if len(channel_events) > 0:
            min_event_passed_time = \
                min([x.get_min() for x in channel_events])
        else:
            min_event_passed_time = -1
        return min_event_passed_time

    def get_maximum_passed_time_event(self):
        channel_events = self.__channel.get_event(PACKET_RETRIEVED)
        if len(channel_events) > 0:
            max_event_passed_time = \
                max([x.get_max() for x in channel_events])
        else:
            max_event_passed_time = -1
        return max_event_passed_time

    def calculate_maximum_exectutinme_time(self):
        execution_events = self.__channel.get_event(EXECTION_TIME)
        nr_execution_events = len(execution_events)

        if not nr_execution_events:
            return -1

        sum_maximum_executiontime = \
            sum([event.get_max() for event in execution_events])

        return sum_maximum_executiontime/nr_execution_events
