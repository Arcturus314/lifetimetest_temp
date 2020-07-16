import matplotlib.pyplot as plt

class DataModel:

    def __init__(self, log_file):
        self.log_file = log+file

        if os.path.exists(log_file):
            self.localdata = pd.read_csv(log_file)
        else:
            self.localdata = pd.DataFrame(columns=["time", "temp"])
            log = open(self.log_filename, "w")
            log.write("time,voltage\n")
            log.close()

    def add_data(self, temp, timestamp):
        self.localdata.loc[len(self.localdata)] = [timestamp, voltage]
        log = open(self.log_filename, "a")
        log.write(str(timestamp) + "," + str(temp) + "\n")
        log.close()

    def fetch_last_temp(self):
        return list(self.localdata["temp"])[-1]

    def plot(self):
        print("Last Temperature:",self.fetch_last_temp())
        if len(self.localdata["temp"]) > 1:
            print("-> Mean Temperature:",statistics.mean(self.localdata["temp"]))
            print("-> StDev Temperature:",statistics.stdev(self.localdata["temp"]))


        initial_time = list(self.localdata["time"])[0]

        plt.scatter([(tval-initial_time)/86400 for tval in list(self.localdata["time"])], self.localdata["temp"], marker="+", s=10)
        plt.title("LED Lifetime Temperature")
        plt.xlabel("Time (Days)")
        plt.ylabel("Temperature (degC)")
        plt.gca().set_axisbelow(True)
        plt.grid()
        plt.savefig("tempplot.pdf")
        plt.close()

