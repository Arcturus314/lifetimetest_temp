# reads voltages from the Arduino

import serial

class ArduinoInterface:

    def __init__(self, interface, num_leds):
        self.port = serial.Serial(interface)
        self.port.flushInput()
        self.num_leds = num_leds

    def update(self):
        '''
        expect line to be 'temp, PD cal intensity, 5V cal voltage, voltage_0, voltage_2, ... voltage_7'
        '''

        data = self.port.readline()
        print(data)

        try:
            datastring = data[0:len(data)-2].decode('utf-8')
            voltages = datastring.split(",")
            voltages.remove('')
            if len(voltages) != self.num_leds+3: return 0
            led_status = []
            for i in range(len(voltages)):
                led_status.append([i, float(voltages[i])])
            return led_status
        except:
            print("Received malformed data") # or any other error
            return 0




