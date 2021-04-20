from src.measurements.base_measurement import BaseMeasurements


class BaseChannelMeasurement(BaseMeasurements):

    def run(self):
        self._measure()
        self._packet_cycle += 1

    def _measure(self):
        raise NotImplementedError
