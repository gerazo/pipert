from src.channel import Channel
from src.channel_calc import ChannelCalc
from src.constants import PACKET_PUSHED


class ChannelManager(object):
    class __ChannelManager(object):
        def __init__(self):
            self.__channels = []
            self.__na_channels = []
            self.__channels_map = []
            self.__should_update_map = False

        def add_packet(self, packet):
            receiver = packet.get_receiver()
            self.__add_reciever(receiver, packet.get_events(), packet.get_id())

            sender = packet.get_sender()
            has_pushed_events = packet.has_event(PACKET_PUSHED)
            self.__add_to_channels_map(receiver, sender, has_pushed_events)

        def __add_reciever(self, receiver_channel, events, packet_id): 
            should_add_reciever = True
            for channel in self.__channels:
                if channel.get_name() == receiver_channel:
                    channel.add_events(events)
                    channel.set_latest_packet_id(packet_id)
                    should_add_reciever = False
                    return
            
            if should_add_reciever:
                c = Channel(receiver_channel, events, packet_id)
                self.__channels.append(c)


        def __add_to_channels_map(self, receiver, sender, has_pushed_events):
            if not has_pushed_events:
                return

            if sender == "N/A":
                sender = "External_" + receiver
                if sender not in self.__na_channels:
                    self.__na_channels.append(sender)
            else:
                channel_names = [c.get_name() for c in self.__channels]
                if sender not in channel_names:
                    c = Channel(sender, [], -1)
                    self.__channels.append(c)

            sender_n_receiver = (sender, receiver) 

            if sender_n_receiver not in self.__channels_map:
                self.__channels_map.append(sender_n_receiver)
                self.__should_update_map = True


        def get_channels_map(self):
            channels_names = [c.get_name() for c in self.__channels]
            unique_channels = channels_names + self.__na_channels
            channels_dict = {c: i for i, c in enumerate(unique_channels)}
            connections = [(channels_dict[s], channels_dict[r]) for (s,r) in 
                            self.__channels_map]

            return unique_channels, connections

        def should_update_map(self):
            val = self.__should_update_map
            self.__should_update_map = False

            return val

        def get_channels(self):
            return self.__channels

        def get_channels_dict(self):
            return [ChannelCalc(channel).get_dict() for channel in
                    self.__channels]

        def get_pipline_thrust_spent_time(self):
            minimum_events_spent_times = \
                [ChannelCalc(x).get_minimum_passed_time_event()
                 for x in self.__channels]
            maximum_events_spent_times = \
                [ChannelCalc(x).get_maximum_passed_time_event()
                 for x in self.__channels]
            while -1 in minimum_events_spent_times:
                minimum_events_spent_times.remove(-1)
            while -1 in maximum_events_spent_times:
                maximum_events_spent_times.remove(-1)
            if(len(minimum_events_spent_times) > 0 and
                    len(maximum_events_spent_times) > 0):
                return max(maximum_events_spent_times)\
                       - min(minimum_events_spent_times)
            else:
                return -2

        def get_pipeline_maximum_Total_execution_time(self):
            channels_maximum_execution_times = \
                [ChannelCalc(x).calculate_maximum_exectutinme_time()
                 for x in self.__channels]
            while -1 in channels_maximum_execution_times:
                channels_maximum_execution_times.remove(-1)
            if(len(channels_maximum_execution_times) > 0):
                return sum(channels_maximum_execution_times)
            else:
                return -1

        def calculate_pipeline_total_execution_time_to_pipeline_time(self):
            pipline_total_time = self.get_pipline_thrust_spent_time()
            pipeline_total_execution_time = \
                self.get_pipeline_maximum_Total_execution_time()
            if(pipline_total_time != -1 and pipline_total_time != -1):
                if(pipline_total_time != 0 and pipline_total_time != 0):
                    return pipeline_total_execution_time / pipline_total_time
                else:
                    return -1

            else:
                return -1

        # def calculate_channel_time_to_read(self, channel):
        #     channelsMap = self.generate_channels_ordered_map()
        #     # bug may happen here because of NA connections
        #     if channel.get_name() in channelsMap:
        #         channelIndex = channelsMap.index(channel.get_name())
        #         if channelIndex == 0:
        #             return 0
        #         else:
        #             previous_channel = channelsMap[channelIndex-1]
        #             channel_events = channel.get_events()[0]
        #             previous_channel = list(
        #                 filter(lambda x: x.get_name() == previous_channel,
        #                        self.__channels))
        #             previous_channel_events =\
        #                 previous_channel[0].get_events()[0]
        #             channel_events_passed_times =\
        #                 [x.get_passed_time() for x in channel_events]
        #             previous_channel_events_passed_times = \
        #                 [x.get_passed_time() for x in previous_channel_events]
        #             if(len(channel_events_passed_times
        #                    ) > 0 and len(previous_channel_events_passed_times
        #                                  ) > 0):
        #                 return\
        #                     ((sum(channel_events_passed_times
        #                           )/len(channel_events_passed_times)
        #                       )-(sum(previous_channel_events_passed_times
        #                              )/len(
        #                         previous_channel_events_passed_times)))
        #             else:
        #                 return 0
        #     else:
        #         return 0


    __instance = None

    def __new__(cls):
        if ChannelManager.__instance is None:
            ChannelManager.__instance = ChannelManager.__ChannelManager()

        return ChannelManager.__instance
