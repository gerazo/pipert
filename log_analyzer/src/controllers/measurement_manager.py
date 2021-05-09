from src.module_getter import ModuleGetter
from src.utils import reduce_points_n_extract_y_axis


class MeasurementManager(object):
    def __init__(self):
        self.__pipeline_measurements_dict = {}
        self.__channel_measuresments = ModuleGetter().get_enabled_channel_measurements()
        self.__pipeline_measuresments = ModuleGetter().get_enabled_pipeline_measurements()

    def run(self):
        self.__run_channel_measurements()
        self.__run_pipeline_measurements()

    def __run_channel_measurements(self):
        for measurement in self.__channel_measuresments:
            measurement.run()

    def __run_pipeline_measurements(self):
        for measurement in self.__pipeline_measuresments:
            measurement_point = measurement.run()
            key = measurement.get_measurement_key()
            if key not in self.__pipeline_measurements_dict:
                self.__pipeline_measurements_dict.update({key: [measurement_point]})
            else:
                self.__pipeline_measurements_dict[key].append(measurement_point)

    def get_pipeline_measurements(self):
        ret_measures = {}
        for measure_name in self.__pipeline_measurements_dict:
            ret_measures.update({measure_name:
                                 reduce_points_n_extract_y_axis(self.__pipeline_measurements_dict[measure_name])})
            self.__pipeline_measurements_dict[measure_name] = []

        return ret_measures
