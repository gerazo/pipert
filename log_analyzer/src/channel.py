from src.utils import flatten_list, reduce_points_n_extract_y_axis


class Channel(object):
    """Represents a channel in the pipeline of the DSP application.

    Attributes:
        name: A string of the name of the channel
        events: A list of the events in the channel
        latest_packet_id: An integer of the id for the last packet's events added to the channel
    """

    def __init__(self, name, events, latest_packet_id):
        self.__name = name
        self.__events = [events]
        self.__flags = {}
        self.__measures = {}
        self.__latest_packet_id = latest_packet_id

    def add_events(self, events):
        """Stores events in the channel

        Args:
            events: A list of events that should be added
        """

        self.__events.append(events)

    def add_measure(self, measure_name, measure_val):
        """Stores measurements in the channel

        Args:
            measure_name: A string of measurement's name
            measure_val: A tuple of integers of measurement's point
        """

        if not self.__measures.get(measure_name):
            self.__measures.update({measure_name: []})

        self.__measures.get(measure_name).append(measure_val)

    def add_flag(self, flag, value):
        """Adds a flag with value if it is not already included, updates flag with value if it exists.
        Args:
            flag: A string of the name of the flag
            value: A boolean value to indicate whether this flag fails or not.
        """

        self.__flags.update({flag: value})

    def get_event(self, event_type):
        """A getter for events of an event type
        Args:
            event_type: A string of the event type

        Returns:
            A list of events of the type given as an input
        """

        return list(filter(lambda x: x.get_type() == event_type,
                           flatten_list(self.get_events())))

    def get_flag(self, flag):
        """A getter for a flag's value
        Args:
            flag: A string of a flag's name

        Returns:
            Return a boolean value to indicate the status of the flag
        """

        return self.__flags[flag]

    def get_flags(self):
        """Clear the events list and a getter for all the flags in the channel
        Returns:
            A dictionary of flags' names and their values
        """

        self.__events = []
        return self.__flags

    def get_measure(self, measure):
        """A getter for a measure y-axis value
        Args:
            measure: A string of the measurement's name

        Returns:
            An integer value
        """

        return self.__measures.get(measure)[-1][1]

    def get_all_measures(self):
        """A getter for the measurements in the channel
        Returns:
            A dictionary of measurements' names and their list of integer or real values
        """

        ret_measures = {}
        for measure_name in self.__measures:
            ret_measures.update({measure_name:
                                 reduce_points_n_extract_y_axis(self.__measures[measure_name])})
            self.__measures[measure_name] = []

        return ret_measures

    def get_name(self):
        """ A getter for the channel's name
        Returns:
            A string
        """

        return self.__name

    def get_events(self):
        """ A getter for the channel's events
        Returns:
            A list of lists of events
        """

        return self.__events

    def get_latest_packet_id(self):
        """ A getter for the last packet's id that was assigned to the channel
        Returns:
            An integer value of the id
        """
        return self.__latest_packet_id

    def set_latest_packet_id(self, latest_packet_id):
        """ A setter for the last packet's id to assign to the channel
        Args:
            latest_packet_id: An integer value
        """
        self.__latest_packet_id = latest_packet_id

    def __str__(self):
        return self.__name

    def __repr__(self):
        return "\n" + str(self) + "\n"
