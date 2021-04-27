from src.measurements.base_measurement import BaseMeasurements


class BasePipelineMeasurement(BaseMeasurements):
    def run(self):
        measurement = self._measure()
        measurement_point = [self._packet_cycle, measurement]
        self._packet_cycle += 1

        return measurement_point

    def _measure(self):
        raise NotImplementedError
