import threading

from src.analyzer_server import AnalyzerServer
from src.gui.application import run
from src.config_reader import ConfigReader

if __name__ == "__main__":
    ip, port = ConfigReader().get_ip_n_port()
    t = threading.Thread(target=AnalyzerServer(ip, port).run,
                         daemon=True)
    t.start()
    run()
