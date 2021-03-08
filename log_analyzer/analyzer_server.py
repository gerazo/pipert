import socket
import measurements_checker
from packet_decoder import PacketDecoder



class AnalyzerServer(object):

    def __init__(self, ip, port):
        self.__ip = ip
        self.__port = port
        self.__output = ""
#channels history dictionar will hold historical data of last n cycle
#events time and numbers per cycle (Ex : how many drops and execution)
        self.channels_history={}

#events statistics are stored here for each channel's event

        self.channels_statistics={}

#this dictionary with hold the calculated flags for each channal
        self.flags={}

    def __configure_server(self):
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.bind((self.__ip, self.__port))
        return sock

    def updateFlags(self):
        channelsNames= self.channels_history.keys()
        for channel in channelsNames:
            if channel not in self.flags:
                #another flags to be added here
                value={channel: {"Frozen":False}}
                self.flags.update(value)


    def __start_server(self):
        s = self.__configure_server()
        while True:
            data, address = s.recvfrom(4096)
            self.__output = data
#            print(PacketDecoder(self.__output).decode_packet(0))
            packet,po=PacketDecoder(self.__output).decode_packet(0);
            if packet.get_receiver() not in self.channels_history:
                self.channels_history[packet.get_receiver()]=[packet]
            else:
                self.channels_history[packet.get_receiver()].append(packet)
                self.channels_history.update({packet.get_receiver():self.channels_history[packet.get_receiver()]})
            self.updateFlags()

            self.flags=measurements_checker.MeasurementsAnalyzer.analyze_channal_freeze_scenario(self.channels_history,self.flags)
            print(self.flags)





    def run(self):
        self.__configure_server()
        self.__start_server()

    def get_output(self):
        return self.__output


