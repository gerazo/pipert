class Event(object):
    def __init__(self, type, log_count, passed_time, min_val, max_val, avg_val):
        self.__type = type
        self.__log_count = log_count
        self.__passed_time = passed_time
        self.__min_val = min_val
        self.__max_val = max_val
        self.__avg_val = avg_val

    def get_type(self):
        return self.__type[4:]

    def get_log_count(self):
        return self.__log_count

    def get_passed_time(self):
        return self.__passed_time

    def get_min(self):
        return self.__min_val

    def get_max(self):
        return self.__max_val

    def get_avg(self):
        return self.__avg_val

    def get_dict(self):
        dict = {"type": self.get_type(),
                "log_count": self.get_log_count(),
                "passed_time": self.get_passed_time(),
                "max": self.get_max(),
                "min": self.get_min(),
                "avg": self.get_avg()}

        return dict

    def __str__(self):
        return "type: {}, log count: {}, time passed: {}, min: {}, max: {}, avg: {}".format(self.get_type(),
                                                                                            self.get_log_count(),
                                                                                            self.get_passed_time(),
                                                                                            self.get_min(),
                                                                                            self.get_max(),
                                                                                            self.get_avg())

    def __repr__(self):
        return "\n" + str(self) + "\n"
