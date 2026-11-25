from machine import Pin
from utime import sleep

pin = Pin("LED", Pin.OUT)

def execute():
    print("LED starts flashing...")
    while True:
        try:
            pin.toggle()
            sleep(0.05) # sleep 1sec
        except KeyboardInterrupt:
            break
    pin.off()
    print("Finished.")


if __name__ == "__main__":
    execute()
