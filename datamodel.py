import matplotlib.pyplot as plt
import os
import pandas as pd
import statistics

class DataModel:

    def __init__(self, log_file):
        self.log_file = log_file

        if os.path.exists(log_file):
            self.localdata = pd.read_csv(log_file)
        else:
            self.localdata = pd.DataFrame(columns=["time", "data"])
            log = open(self.log_file, "w")
            log.write("time,data\n")
            log.close()

    def add_data(self, data, timestamp):
        self.localdata.loc[len(self.localdata)] = [timestamp, data]
        log = open(self.log_file, "a")
        log.write(str(timestamp) + "," + str(data) + "\n")
        log.close()

    def fetch_last_temp(self):
        return list(self.localdata["data"])[-1]

    def plot(self):
        print(self.log_file, "Last Value:",self.fetch_last_temp())

        if len(self.localdata["data"]) > 1:
            print("-> Mean Parameter Value:",statistics.mean(self.localdata["data"]))
            print("-> StDev Parameter Value:",statistics.stdev(self.localdata["data"]))


        initial_time = list(self.localdata["time"])[0]

        xvals = [(tval-initial_time)/86400 for tval in list(self.localdata["time"])]
        yvals = self.localdata["data"]


        plt.scatter(xvals, yvals, marker="+", s=10)
        plt.title("LED Lifetime Parameter")
        plt.xlabel("Time (Days)")
        plt.ylabel("Parameter Value")
        plt.gca().set_axisbelow(True)
        plt.grid()
        plt.savefig(self.log_file.split("/")[-1] + ".pdf")
        plt.close()

