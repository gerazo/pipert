class Packet(object):
    """Represents a packet received from the profiler

    Attributes:
        receiver: A string of the name of the receiver's channel
        sender: A string of the name of the sender's channel
        events: A list of events
    """

    def __init__(self, receiver, sender, events):
        self.__receiver = receiver
        self.__sender = sender
        self.__events = events
        self.__id = None

    def add_events(self, events):
        """Adds new events to the packet

        Args:
            events: A list of events
        """

        self.__events += events

    def get_event_count(self, event_type):
        """A getter for the log count for an event type

        Args:
            event_type: A string of the event type

        Returns:
            An integer of the log count or if the event does not exist, it returns zero
        """

        for event in self.get_events():
            if event.get_type() == event_type:
                return event.get_log_count()

        return 0

    def get_receiver(self):
        """A getter for the receiver channel's name

        Returns:
            A string of the receiver channel's name
        """

        return self.__receiver

    def get_sender(self):
        """A getter for the sender channel's name

        Returns:
            A string of the sender's name
        """

        return self.__sender[4:]

    def get_events(self):
        """A getter for the events in the packet

        Returns:
            A list of events
        """

        return self.__events

    def get_id(self):
        """A getter for the packet's id

        Returns:
            An integer value of the packet's id
        """

        return self.__id

    def get_dict(self):
        """A getter for the packet in dictionary format

        Returns:
            A dictionary contains all the information of the packet
        """

        packet_dict = {"receiver_channel": self.get_receiver(),
                       "sender_channel": self.get_sender(),
                       "events": [event.get_dict() for event in self.get_events()]}
        return packet_dict

    def set_id_for_events(self):
        """Sets packet's id for all the events in the packet
        """

        [event.set_packet_id(self.__id) for event in self.__events]

    def set_id(self, packet_id):
        """A setter for the packet's id

        Args:
            packet_id: An integer value for the packet's id
        """

        self.__id = packet_id

    def __str__(self):
        return "Receiver: {}\nSender: " \
               "{}\nEvents: {}".format(self.get_receiver(),
                                       self.get_sender(),
                                       self.get_events())

    def __repr__(self):
        return "\n" + str(self) + "\n"
