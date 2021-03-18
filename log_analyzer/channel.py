FROZEN = "FROZEN"
HIGHDROPRATE="HIGHDROPRATE"
PACKETS_THRESHOULD = 20


class Channel(object):
    def __init__(self, name, events, latest_packet_id):
        self.__name = name
        self.__events = [events]
        self.__flags = {FROZEN: False,HIGHDROPRATE:False}
        self.__packet_count = 1
        self.__latest_packet_id = latest_packet_id

    def add_events(self, events):
        if self.__packet_count % PACKETS_THRESHOULD == 0:
            self.__events.clear()

        print(self.__packet_count)
        self.__packet_count += 1
        self.__events.append(events)

    def update_flag(self, flag, value):
        val = self.__flags.get(flag)
        if val is None:
            raise ValueError("Not a correct flag")

        self.__flags[flag] = value

    def get_flag(self, flag):
        return self.__flags[flag]

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
    # return the flag if the dropping rate is high
    # it will return -1 if the channel if both number of executed and dropped events are 0
    # if there is no execution only dropping it will return 1
    def drop_rate_calculator(self):
        result=-2
        number_of_dropped_events=len(self.get_drop_events())
        number_of_executed_events=len(self.get_execution_events())
        if(number_of_dropped_events==0 and number_of_executed_events==0):
            result=-1
        else:
            if(number_of_executed_events==0):
                result=1
            else:
                if(number_of_dropped_events==0):
                    result=0
                else:
                    result=number_of_dropped_events/number_of_executed_events

        return result
    def get_fill_events(self):
        return list(filter(lambda x: x.get_type()== "Fill Time", self.__flatten_list(self.get_events())))

    def get_pushed_events(self):
        return list(filter(lambda x: x.get_type()== "Packet Pushed", self.__flatten_list(self.get_events())))

    def get_retrieved_events(self):
        return list(filter(lambda x: x.get_type()== "Packet Retrieved", self.__flatten_list(self.get_events())))

    def get_execution_events(self):
        return list(filter(lambda x: x.get_type()== "Execution Time", self.__flatten_list(self.get_events())))

    def get_read_events(self):
        return list(filter(lambda x: x.get_type()== "Read Time", self.__flatten_list(self.get_events())))

    def get_drop_events(self):
        return list(filter(lambda x: x.get_type()== "Packet Dropped", self.__flatten_list(self.get_events())))



    def __flatten_list(self, channels_list):
        return [item for sublist in channels_list for item in sublist]

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
