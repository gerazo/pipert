from src.checkers.base_checker import BaseChecker
from src.controllers.channels_manager import ChannelsManager
from src.constants import HIGH_CHANNEL_TIME_TO_BUFFER, CHANNEL_TIME_TO_BUFFER_AVERAGE_THRESHOLD


# This class contains the logic to check time to buffer of the channels
# (average time spent from times packets released from previous channel to the times to buffer
# of current channel) and set its flag value channels
class TimeToBufferAverageChecker(BaseChecker):
    """ get the time to buffer measure value from the channel
       if its larger than defined CHANNEL_TIME_TO_BUFFER_AVERAGE_THRESHOLD in the config file
          then we set the flag for HIGH_CHANNEL_TIME_TO_BUFFER  in the channel value to true
       else we set flag for HIGH_CHANNEL_TIME_TO_BUFFER  in the channel value to false

       Args: none

       Returns: none
    """
    def run(self):
        for channel in ChannelsManager().get_channels():
            time_to_buffer = channel.get_measure(self._measure_key)
            if (time_to_buffer > self._parameters[CHANNEL_TIME_TO_BUFFER_AVERAGE_THRESHOLD]):
                channel.update_flag(HIGH_CHANNEL_TIME_TO_BUFFER, True)
            else:
                channel.update_flag(HIGH_CHANNEL_TIME_TO_BUFFER, False)
