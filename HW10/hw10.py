import pgzrun
import serial

# ME433 HW10 - pygame zero version
# Reads "ax,ay" lines from the Pico over serial and moves a ball with tilt.

PORT = 'COM6' # for some reason my pico has been using com6, can change to com5 for normal picos, can check in vscode serial monitor
BAUD = 115200 # match the baud rate in your pico code

WIDTH = 600     # pgz reads these globals to size the window
HEIGHT = 600

ser = serial.Serial(PORT, BAUD, timeout=0)

# ball state
ball_x = WIDTH / 2
ball_y = HEIGHT / 2
vx = 0.0
vy = 0.0

def update():
    global ball_x, ball_y, vx, vy

    # read the most recent complete line available
    try:
        line = ser.readline().decode().strip()
        if line:
            parts = line.split(',')
            if len(parts) == 2:
                ax = float(parts[0])
                ay = float(parts[1])
                vx += ax * 2.0
                vy += ay * 2.0
    except (ValueError, UnicodeDecodeError):
        pass

    # friction + move
    vx *= 0.95
    vy *= 0.95
    ball_x += vx
    ball_y += vy

    # bounce off walls
    if ball_x < 20:
        ball_x = 20; vx = -vx * 0.5
    if ball_x > WIDTH - 20:
        ball_x = WIDTH - 20; vx = -vx * 0.5
    if ball_y < 20:
        ball_y = 20; vy = -vy * 0.5
    if ball_y > HEIGHT - 20:
        ball_y = HEIGHT - 20; vy = -vy * 0.5

def draw():
    screen.fill((20, 20, 30))
    screen.draw.filled_circle((int(ball_x), int(ball_y)), 20, (80, 200, 255))

pgzrun.go()
