from src.config_reader import ConfigReader


class ModuleGetter(object):
    """Imports modules dynamically, modules like checkers and measurements
    """

    def get_enabled_checkers(self):
        """ A getter for the enabled checkers and it sets the parameter and measurement key for them

        Returns:
            A list of enabled checkers
        """

        enabled_checkers = []
        for checker in ConfigReader().get_enabled_checkers():
            c = self.__import_checker(checker["name"])
            c.set_parameters(checker["parameters"])
            c.set_measure_key(checker["measure_key"])
            enabled_checkers.append(c)

        return enabled_checkers

    def get_enabled_channel_measurements(self):
        """A getter for enabled channel's measurements, it sets the measurement key for them

        Returns:
            A list of the enabled channel's measurements
        """

        enabled_measurements = []
        for measurement in ConfigReader().get_enabled_channel_measurements():
            m = self.__import_measurement("channel_measurements", measurement[0])
            m.set_measurement_key(measurement[1])
            enabled_measurements.append(m)

        return enabled_measurements

    def get_enabled_pipeline_measurements(self):
        """A getter for enabled pipeline's measurements, it sets the measurement key for them

        Returns:
            A list of the pipeline's measurements
        """

        enabled_measurements = []
        for measurement in ConfigReader().get_enabled_pipeline_measurements():
            m = self.__import_measurement("pipeline_measurements", measurement[0])
            m.set_measurement_key(measurement[1])
            enabled_measurements.append(m)

        return enabled_measurements

    def __import_measurement(self, measurement_type, measurement_name):
        """Imports a measurement

        Args:
            measurement_type: A string of the measurement's type
            measurement_name: A string of the measurement's name

        Returns:
            A measurement instance
        """

        class_name = self.__get_class_name(measurement_name)
        mod = __import__("src.measurements." + measurement_type + "." + measurement_name, fromlist=[class_name])
        measurement = getattr(mod, class_name)

        return measurement()

    def __import_checker(self, checker_name):
        """Imports a checker

        Args:
            checker_name: A string of the checker's name

        Returns:
            A checker instance
        """

        class_name = self.__get_class_name(checker_name)
        mod = __import__("src.checkers." + checker_name, fromlist=[class_name])
        checker = getattr(mod, class_name)

        return checker()

    @staticmethod
    def __get_class_name(file_name):
        """Parsing the name of the class from the class's file name
        
        Args:
            file_name: A string of the class's file name

        Returns:
            A string of the class name
        """

        class_name = file_name[0].upper()
        i = 1
        while i < len(file_name):
            if file_name[i] != "_":
                class_name += file_name[i]
            else:
                i += 1
                class_name += file_name[i].upper()
            i += 1

        return class_name
