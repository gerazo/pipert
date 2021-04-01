from src.constants import PACKET_DROPPED, EXECTION_TIME, READ_TIME

class ChannelCalc(object):
    def __init__(self, channel):
        self.__channel = channel

    def calculate_drop_rate(self):
        nr_dropped_event = len(self.__channel.get_event(PACKET_DROPPED))
        nr_executed_event = len(self.__channel.get_event(EXECTION_TIME))
        if (not nr_dropped_event) and (not nr_executed_event):
            return -1

        if not nr_executed_event:
            return 1

        if not nr_dropped_event:
            return 0

        return nr_dropped_event/nr_executed_event

    def calculate_drop_ratio(self):
        nr_dropped_event = len(self.__channel.get_event(PACKET_DROPPED))
        nr_read_event = len(self.__channel.get_event(READ_TIME))

        if (not nr_dropped_event) and (not nr_read_event):
            return -1

        if not nr_read_event:
            return 1

        if not nr_dropped_event:
            return 0

        return nr_dropped_event/nr_read_event

    def calculate_exectutinme_time(self):
        execution_events = self.__channel.get_event(EXECTION_TIME)
        nr_execution_events = len(execution_events)

        if not nr_execution_events:
            return -1

        sum_average = sum([event.get_avg() for event in execution_events])

        return sum_average/nr_execution_events

