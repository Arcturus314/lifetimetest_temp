import matplotlib.pyplot as plt
import ledmodel
import arduinointerface
import tempmodel
import time

log_dir    = "lifetimelogging"
log_prefix = "logLED"
numLED = 8

LEDs = []
logger = arduinointerface.ArduinoInterface("/dev/ttyACM0", numLED)
tempmodel = tempmodel.TempModel(log_dir + "/temp_log.csv")

for i in range(numLED): LEDs.append(ledmodel.LEDModel(log_dir+"/"+log_prefix+str(i)+".csv", i))

num_samples = 0

while True:

    num_samples += 1

    logger_data = logger.update()

    if logger_data != 0:

        currtime = time.time()

        led_status  = logger_data[1:]
        temp        = logger_data[0]

        tempmodel.add_data(temp, currtime)

        for identifier, voltage in led_status:
            LEDs[identifier].add_data(voltage, currtime)

        if num_samples % 1 == 0:
            tempmodel.plot()
            for led in LEDs: led.plot()
            print("\n")

    else: print("Received incomplete data")
