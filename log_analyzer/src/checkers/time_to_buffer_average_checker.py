from src.checkers.base_checker import BaseChecker
from src.channel_manager import ChannelManager
from src.constants import CHANNEL_TIME_TO_BUFFER, HIGH_CHANNEL_TIME_TO_BUFFER,\
    CHANNEL_TIME_TO_BUFFER_AVERAGE_THRESHOLD, PACKET_RETRIEVED, EXECTION_TIME


# This class contains the logic to check time to buffer of the channels
# (average time spent from times packets released from previous channel to the times to buffer
# of current channel) and set its flag value channels
class TimeToBufferAverageChecker(BaseChecker):
    """get the time to buffer measure value from the channel
       if its larger than defined CHANNEL_TIME_TO_BUFFER_AVERAGE_THRESHOLD in the config file
          then we set the flag for HIGH_CHANNEL_TIME_TO_BUFFER  in the channel value to true
       else we set flag for HIGH_CHANNEL_TIME_TO_BUFFER  in the channel value to false

       Parameters: none

       Returns: none
    """
    def run(self):
        for channel in ChannelManager().get_channels():
            time_to_buffer = channel.get_measure(self._measure_key)
            if (time_to_buffer > self._parameters[CHANNEL_TIME_TO_BUFFER_AVERAGE_THRESHOLD]):
                channel.update_flag(HIGH_CHANNEL_TIME_TO_BUFFER, True)
            else:
                channel.update_flag(HIGH_CHANNEL_TIME_TO_BUFFER, False)
