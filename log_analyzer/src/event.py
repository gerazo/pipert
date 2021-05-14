class Event(object):
    """Represents an event in a packet from the profiler

    Attributes:
        event_type: A string of the event's type
        log_count: An integer of event's log count
        passed_time: An integer value of event's passed time in milliseconds
        min_val: A real value of the minimum value of time that the event took to complete
        max_val: A real value of the maximum value of time that the event took to complete
        avg_val: A real value of the average value of time that the event took to complete
    """

    def __init__(self, event_type, log_count, passed_time,
                 min_val, max_val, avg_val):
        self.__type = event_type
        self.__log_count = log_count
        self.__passed_time = passed_time
        self.__min_val = min_val
        self.__max_val = max_val
        self.__avg_val = avg_val
        self.__packet_id = None

    def get_type(self):
        """A getter for the event's type

        Returns:
            A string of the event's type
        """

        return self.__type[4:]

    def get_log_count(self):
        """A getter for the event's log count

        Returns:
            An integer of the log count
        """

        return self.__log_count

    def get_passed_time(self):
        """A getter for the event's passed time

        Returns:
            An integer of the passed time
        """

        return self.__passed_time

    def get_min(self):
        """A getter of the event's minimum time

        Returns:
            A real of event's minimum time
        """

        return self.__min_val

    def get_max(self):
        """A getter of the event's maximum time

        Returns:
            A real of event's maximum time
        """

        return self.__max_val

    def get_avg(self):
        """A getter of the event's average time

        Returns:
            A real of event's average time
        """

        return self.__avg_val

    def get_packet_id(self):
        """A getter for the packet's id

        Returns:
            An integer of the packet's id
        """

        return self.__packet_id

    def get_dict(self):
        """Returns the event in a dictionary format

        Returns:
            A dictionary of the event's information
        """

        event_dict = {"type": self.get_type(),
                      "log_count": self.get_log_count(),
                      "passed_time": self.get_passed_time(),
                      "max": self.get_max(),
                      "min": self.get_min(),
                      "avg": self.get_avg()}

        return event_dict

    def set_packet_id(self, packet_id):
        """Sets a packet's id for the event

        Args:
            packet_id: An integer of the packet's id
        """

        self.__packet_id = packet_id

    def __str__(self):
        return "type: {}, log count: {}, " \
               "time passed: {}, min: {}, " \
               "max: {}, avg: {}".format(self.get_type(),
                                         self.get_log_count(),
                                         self.get_passed_time(),
                                         self.get_min(),
                                         self.get_max(),
                                         self.get_avg())

    def __repr__(self):
        return "\n" + str(self) + "\n"
