class Packet(object):
    def __init__(self, receiver, sender, events):
        self.__receiver = receiver
        self.__sender = sender
        self.__events = events
        self.__id = None

    def has_event(self, event_type):
        for event in self.get_events():
            if event.get_type() == event_type:
                return True

        return False

    def get_receiver(self):
        return self.__receiver

    def get_sender(self):
        return self.__sender[4:]

    def get_events(self):
        return self.__events

    def add_events(self, events):
        self.__events += events

    def set_id(self, id):
        self.__id = id

    def set_id_for_events(self):
        [event.set_packet_id(self.__id) for event in self.__events]

    def get_id(self):
        return self.__id

    def get_dict(self):
        dict = {"receiver_channel": self.get_receiver(),
                "sender_channel": self.get_sender(),
                "events": [event.get_dict() for event in self.get_events()]}
        return dict

    def __str__(self):
        return "Receiver: {}\nSender: " \
               "{}\nEvents: {}".format(self.get_receiver(),
                                       self.get_sender(),
                                       self.get_events())

    def __repr__(self):
        return "\n" + str(self) + "\n"
