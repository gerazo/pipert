class Packet(object):
    def __init__(self, receiver, sender, events):
        self.__receiver = receiver
        self.__sender = sender
        self.__events = events

    def get_receiver(self):
        return self.__receiver

    def get_sender(self):
        return self.__sender[4:]

    def get_events(self):
        return self.__events

    def __str__(self):
        return "Receiver: {}\nSender: {}\nEvents: {}".format(self.get_receiver(), self.get_sender(), self.get_events())

    def __repr__(self):
        return "\n" + str(self) + "\n"