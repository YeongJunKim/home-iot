import snowboydecoder
import sys
import signal
import datetime

interrupted = False


def signal_handler(signal, frame):
    global interrupted

    print("signal haldler")
    print(frame)
    print(signal)

    interrupted = True


def detected_callback():
    print("Detected callback called")


def interrupt_callback():
    # print(f"Jarvis called")
    global interrupted
    return interrupted


if len(sys.argv) == 1:
    print("Error: need to specify model name")
    print("Usage: python demo.py your.model")
    sys.exit(-1)

model = sys.argv[1]

# capture SIGINT signal, e.g., Ctrl+C
signal.signal(signal.SIGINT, signal_handler)

detector = snowboydecoder.HotwordDetector(model, sensitivity=0.8)
print('Listening... Press Ctrl+C to exit')

# main loop
detector.start(detected_callback=detected_callback,
               interrupt_check=interrupt_callback,
               sleep_time=0.03)

detector.terminate()
