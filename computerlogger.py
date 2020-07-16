import matplotlib.pyplot as plt
import ledmodel
import arduinointerface
import datamodel
import time
import os

log_dir    = "lifetimelogging"
log_prefix = "logLED"
numLED = 8

LEDs = []
logger = arduinointerface.ArduinoInterface("/dev/ttyACM0", numLED)
tempmodel = datamodel.DataModel(log_dir + "/temp_log.csv")
ledcontrolmodel = datamodel.DataModel(log_dir + "/led_control_log.csv")
voltagemodel = datamodel.DataModel(log_dir + "/voltage_log.csv")


for i in range(numLED): LEDs.append(ledmodel.LEDModel(log_dir+"/"+log_prefix+str(i)+".csv", i))

num_samples = 0

while True:

    num_samples += 1

    logger_data = logger.update()

    if logger_data != 0:

        currtime = time.time()

        temp        = logger_data[0]
        led_control = logger_data[1]
        involtage   = logger_data[2]
        led_status  = logger_data[3:]

        tempmodel.add_data(temp[1], currtime)
        ledcontrolmodel.add_data(led_control[1], currtime)
        voltagemodel.add_data(involtage[1], currtime)

        for identifier, voltage in led_status:
            LEDs[identifier-3].add_data(voltage, currtime)

        if num_samples % 1 == 0:
            tempmodel.plot()
            ledcontrolmodel.plot()
            voltagemodel.plot()
            for led in LEDs: led.plot()
            print("\n")

    else: print("Received incomplete data")
