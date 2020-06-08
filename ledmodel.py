# models an LED for the purpose of lifetime testing

import os.path
import time
import pandas as pd
import matplotlib.pyplot as plt
import statistics

class LEDModel:

    def __init__(self, log_file, identifier):
        self.log_filename = log_file
        self.identifier = identifier

        if os.path.exists(log_file):
            self.localdata = pd.read_csv(log_file)
        else:
            self.localdata = pd.DataFrame(columns=["time", "voltage"])
            log = open(self.log_filename, "w")
            log.write("time,voltage\n")
            log.close()

    def add_data(self, voltage, timestamp):
        self.localdata.loc[len(self.localdata)] = [timestamp, voltage]
        log = open(self.log_filename, "a")
        log.write(str(timestamp) + "," + str(voltage) + "\n")
        log.close()

    def fetch_last_voltage(self):
        return list(self.localdata["voltage"])[-1]

    def plot(self):
        print("LED:",self.identifier)
        print("-> Last Voltage:",self.fetch_last_voltage())
        if len(self.localdata["voltage"]) > 1:
            print("-> Mean Voltage:",statistics.mean(self.localdata["voltage"]))
            print("-> StDev Voltage:",statistics.stdev(self.localdata["voltage"]))


        initial_time = list(self.localdata["time"])[0]

        plt.scatter([(tval-initial_time)/86400 for tval in list(self.localdata["time"])], self.localdata["voltage"], marker="+", s=10)
        plt.title("LED "+str(self.identifier)+" Lifetime")
        plt.xlabel("Time (Days)")
        plt.ylabel("Detector Voltage (V)")
        plt.gca().set_axisbelow(True)
        plt.grid()
        plt.savefig("led"+str(self.identifier)+"plot.pdf")
        plt.close()
