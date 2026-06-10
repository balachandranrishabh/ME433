import pgzrun
import serial
import math

# ME433 HW17 - graphics for paddle position + force
# Reads "angle,force" from the Pico. Draws a rotating paddle for the angel + bar for force

PORT = 'COM6'

WIDTH = 600
HEIGHT = 600

ser = serial.Serial(PORT, 115200, timeout=0)

angle = 0.0      
force = 0.0     
force0 = None    # set to first reading to zero the scale

def update():
    global angle, force, force0
    try:
        line = ser.readline().decode().strip()
        if line:
            parts = line.split(',')
            if len(parts) == 2:
                angle = float(parts[0])
                f = float(parts[1])
                if force0 is None:
                    force0 = f          # tare to first reading
                force = f - force0
    except (ValueError, UnicodeDecodeError):
        pass

def draw():
    screen.fill((20, 20, 30))


    # making paddle a line
    cx, cy = WIDTH / 4, HEIGHT / 2
    theta = (angle / 4096.0) * 2 * math.pi      # encoder -> radians
    length = 150
    ex = cx + length * math.cos(theta)
    ey = cy + length * math.sin(theta)
    screen.draw.line((cx, cy), (ex, ey), (80, 200, 255))
    screen.draw.filled_circle((cx, cy), 10, (200, 200, 200))

    # force bar changes with force
    bar_h = min(abs(force) / 2000.0 * 300, 300)   # scaling down force to bar height, max 300
    color = (255, 100, 100) if force > 0 else (100, 100, 255)
    screen.draw.filled_rect(Rect((40, HEIGHT - 40 - bar_h), (40, bar_h)), color)
    screen.draw.text("force", (30, HEIGHT - 30))
    screen.draw.text("position", (cx - 30, 30))

pgzrun.go()
