import serial
import os



port = "/dev/serial0"

ser = serial.Serial(port, 9600, timeout=1)

filename = "test.txt"

fsize = os.path.getsize(filename)

sent = 0

with open(filename, "rb") as f:
  while True:
    data = f.read(64)
    if not data:
      break

    ser.write(data)
    sent += len(data)

    print("Sent: ", sent, "/", fsize)

ser.close()

print("File Sent")



