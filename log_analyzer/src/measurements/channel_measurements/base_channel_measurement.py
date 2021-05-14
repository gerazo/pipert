from src.measurements.base_measurement import BaseMeasurements


class BaseChannelMeasurement(BaseMeasurements):

    def run(self):
        for channel in self._channel_manager.get_channels():
            measure = self._measure(channel)
            channel.add_measure(self._measurement_key, [self._packet_cycle, measure])

        self._packet_cycle += 1

    def _measure(self, channel):
        raise NotImplementedError
