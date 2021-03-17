import unittest
import socket
import threading
from time import sleep

from analyzer_server import AnalyzerServer


class TestAnalyzerServer(unittest.TestCase):

    def test_when_client_sends_server_should_recieve(self):
        # Given
        ip = "127.0.0.1"
        port = 8000
        server = AnalyzerServer(ip, port)
        # When
        t = threading.Thread(target=server.run, daemon=True)
        t.start()
        sleep(0.001)
        self.__send_data(ip, port, "message")
        # Then
        self.assertEqual("message", server.get_output())

    @staticmethod
    def __send_data(ip, port, msg):
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.sendto(msg.encode(), (ip, port))
