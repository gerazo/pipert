from src.config_reader import ConfigReader


class ModuleGetter(object):
    def get_enabled_checkers(self):
        enabled_checkers = []
        for checker in ConfigReader().get_enabled_checkers():
            c = self.__import_checker(checker["name"])
            c.set_parameters(checker["parameters"])
            c.set_measure_key(checker["measure_key"])
            enabled_checkers.append(c)

        return enabled_checkers

    def get_enabled_channel_measurements(self):
        enabled_measurements = []
        for measurement in ConfigReader().get_enabled_channel_measurements():
            m = self.__import_measurement("channel_measurements", measurement[0])
            m.set_measurement_key(measurement[1])
            enabled_measurements.append(m)

        return enabled_measurements

    def get_enabled_pipeline_measurements(self):
        enabled_measurements = []
        for measurement in ConfigReader().get_enabled_pipeline_measurements():
            m = self.__import_measurement("pipeline_measurements", measurement[0])
            m.set_measurement_key(measurement[1])
            enabled_measurements.append(m)

        return enabled_measurements

    def __import_measurement(self, measurement_type, measurement_name):
        class_name = self.__get_class_name(measurement_name)
        mod = __import__("src.measurements." + measurement_type + "." + measurement_name, fromlist=[class_name])
        measurement = getattr(mod, class_name)

        return measurement()


    def __import_checker(self, checker_name):
        class_name = self.__get_class_name(checker_name)
        mod = __import__("src.checkers."+checker_name, fromlist=[class_name])
        checker = getattr(mod, class_name)

        return checker()

    def __get_class_name(self, checker_name):
        class_name = checker_name[0].upper()
        i = 1
        while i < len(checker_name):
            if checker_name[i] != "_":
                class_name += checker_name[i]
            else:
                i += 1
                class_name += checker_name[i].upper()
            i += 1

        return class_name
