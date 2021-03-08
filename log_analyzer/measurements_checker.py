


class MeasurementsAnalyzer:



    def analyze_channal_freeze_scenario(channelsHistory,flags):
        channelsNames=channelsHistory.keys()
        for channel in channelsNames:
            packets=channelsHistory[channel]
            lastNRecords=packets[-20:]
            noEventsSent=True;
            for event in lastNRecords:
                noEventsSent=noEventsSent and (len(event.get_events())==0)
                channelEventsFlags=flags[channel]
                frozenFlagValue={"Frozen":noEventsSent}
                channelEventsFlags.update(frozenFlagValue)
            flags.update({channel:channelEventsFlags})
        return flags









