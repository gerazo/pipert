from src.utils import flatten_list
from rdp import rdp
from src.constants import (FROZEN, HIGH_DROP_RATE, HIGH_DROP_RATIO,
                           PACKETS_THRESHOULD, PACKET_DROPPED, EXECTION_TIME,
                           READ_TIME, HIGH_EXECUTION_TIME, HIGH_READ_TIME,
                           HIGH_FILL_TIME,
                           HIGH_CHANNEL_TIME_TO_BUFFER)


class Channel(object):
    def __init__(self, name, events, latest_packet_id):
        self.__name = name
        self.__events = [events]
        self.__flags = {FROZEN: False, HIGH_DROP_RATE: False,
                        HIGH_DROP_RATIO: False,
                        HIGH_EXECUTION_TIME: False,
                        HIGH_READ_TIME: False,
                        HIGH_READ_TIME: False,
                        HIGH_FILL_TIME: False,
                        HIGH_CHANNEL_TIME_TO_BUFFER:False}
        self.__measures = {}
        self.__packet_count = 1
        self.__latest_packet_id = latest_packet_id

    def add_events(self, events):
        if self.__packet_count > PACKETS_THRESHOULD:
            self.__events.clear()
            self.__packet_count = 0

        self.__packet_count += 1
        self.__events.append(events)

    def add_measure(self, measure_name, measure_val):
        if not self.__measures.get(measure_name):
            self.__measures.update({measure_name: []})

        self.__measures.get(measure_name).append(measure_val)

    def update_flag(self, flag, value):
        val = self.__flags.get(flag)
        if val is None:
            raise ValueError("{} Not a correct flag".format(flag))

        self.__flags[flag] = value

    def get_event(self, event_type):
        return list(filter(lambda x: x.get_type() == event_type,
                           flatten_list(self.get_events())))

    def get_flag(self, flag):
        return self.__flags[flag]

    def get_flags(self):
        return self.__flags

    def get_measures(self):
        ret_measures = {}
        for measure_name in self.__measures:
            ret_measures.update({measure_name:\
                self.__prepare_points_for_drawing(self.__measures[measure_name])})
            self.__measures[measure_name] = []

        return ret_measures

    def __prepare_points_for_drawing(self, points):
        minimized_points = rdp(points, epsilon=0.5)
        ret_points = [None] * 10
        for point in minimized_points:
            index = int((point[0]%10)-1)
            ret_points[index] = point[1]

        return ret_points

    def get_name(self):
        return self.__name

    def get_events(self):
        return self.__events

    def get_packet_count(self):
        return self.__packet_count

    def get_latest_packt_id(self):
        return self.__latest_packet_id

    def set_latest_packet_id(self, latest_packet_id):
        self.__latest_packet_id = latest_packet_id

    def __str__(self):
        return self.__name + " " + str(self.__packet_count)

    def __repr__(self):
        return "\n" + str(self) + "\n"
