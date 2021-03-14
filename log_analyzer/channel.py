FROZEN = "FROZEN"
PACKETS_THRESHOULD = 20


class Channel(object):
    def __init__(self, name, events):
        self.__name = name
        self.__events = events
        self.__flags = {FROZEN: False}
        self.__packet_count = 1

    def add_events(self, events):
        if self.__packet_count % PACKETS_THRESHOULD == 0:
            self.__events.clear()

        print(self.__packet_count)
        self.__packet_count += 1
        self.__events.append(events)

    def update_flag(self, flag, value):
        flag = self.__flags[flag]
        if flag is None:
            raise ValueError("Not a correct flag")

        flag = value

    def get_name(self):
        return self.__name

    def get_events(self):
        return self.__events

    def get_packet_count(self):
        return self.__packet_count

    def get_dict(self):
        dict = {
                "name": self.__name,
                "flags": self.__flags
                }

        return dict

    def __str__(self):
        return self.__name + " " + str(self.__packet_count)

    def __repr__(self):
        return "\n" + str(self) + "\n"
