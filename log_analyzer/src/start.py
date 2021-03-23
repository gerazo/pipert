import threading

from analyzer_server import AnalyzerServer
from application import run

if __name__ == "__main__":
    t = threading.Thread(target=AnalyzerServer('127.0.0.1', 8000).run,
                         daemon=True)
    t.start()
    run()
