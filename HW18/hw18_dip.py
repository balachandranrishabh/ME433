import numpy as np
import matplotlib.pyplot as plt

# ME433 HW18 - Haptic effect: a DIP
#
# A "dip" is a spot the paddle gets pulled INTO! Basically the opposite of a bump. You can feel it as a "sticky" spot that you have to pull through, and once you do, it snaps back to the center.
#
# We model the dip as a restoring force that is strongest a little way out
# from center and fades both at the center (0 force) and far away (escaped).
# This is the derivative of a "well" potential - a smooth pull-back.
#
# Force is normalized to +-1.

# displacement from the center of the dip, normalized to +-1
x = np.linspace(-1, 1, 500)

# width of the dip (how far the pull reaches)
w = 0.4

# restoring force: pulls back toward center (x=0).
# f = -x/w * exp(-(x/w)^2/2)  so that it's zero at center pushes back toward center, and fades to zero far away.
force = -(x / w) * np.exp(-(x / w)**2 / 2)

# normalize so the peak magnitude is exactly 1
force = force / np.max(np.abs(force))

plt.figure(figsize=(7, 5))
plt.plot(x, force, 'r', linewidth=2)
plt.axhline(0, color='gray', linewidth=0.5)
plt.axvline(0, color='gray', linewidth=0.5)

plt.xlabel('displacement from dip center (normalized)')
plt.ylabel('desired force (normalized)')
plt.title('Haptic "dip" - force vs displacement')

plt.grid(True, alpha=0.3)
plt.tight_layout()
plt.savefig('hw18_dip.png', dpi=120)
plt.show()
