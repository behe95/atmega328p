import serial
import time
import keyboard
import threading


ser = serial.Serial('COM3', 9600, timeout=1)

running = True


def check_keyboard():
    global running
    keyboard.wait('x')
    running = False


threading.Thread(target=check_keyboard, daemon=True).start()

filename = "received.txt"

with open(filename, "wb") as file:

    try:

        print("Receiving... press x to stop")

        while running:

            data = ser.read(64)

            if data:
                file.write(data)
                print("received:", len(data), "bytes")

    finally:

        ser.close()

        print("Serial port closed")
        print("Saved:", filename)
