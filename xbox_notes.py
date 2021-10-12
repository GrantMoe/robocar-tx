{
    ('EV_SYN', 0): [
        ('SYN_REPORT', 0),
        ('SYN_CONFIG', 1), 
        ('SYN_DROPPED', 3), 
        ('?', 4)], 
        ('EV_KEY', 1): [('KEY_BACK', 158), 
        ('KEY_HOMEPAGE', 172), 
        (['BTN_A', 'BTN_GAMEPAD', 'BTN_SOUTH'], 304), 
        (['BTN_B', 'BTN_EAST'], 305), ('BTN_C', 306), 
        (['BTN_NORTH', 'BTN_X'], 307), 
        (['BTN_WEST', 'BTN_Y'], 308), 
        ('BTN_Z', 309), 
        ('BTN_TL', 310), 
        ('BTN_TR', 311), 
        ('BTN_TL2', 312), 
        ('BTN_TR2', 313), 
        ('BTN_SELECT', 314), 
        ('BTN_START', 315), 
        ('BTN_MODE', 316), 
        ('BTN_THUMBL', 317), 
        ('BTN_THUMBR', 318)
        ], 
    ('EV_ABS', 3): [
        (('ABS_X', 0), AbsInfo(value=33427, min=0, max=65535, fuzz=255, flat=4095, resolution=0)), 
        (('ABS_Y', 1), AbsInfo(value=33999, min=0, max=65535, fuzz=255, flat=4095, resolution=0)), 
        (('ABS_Z', 2), AbsInfo(value=31645, min=0, max=65535, fuzz=255, flat=4095, resolution=0)), 
        (('ABS_RZ', 5), AbsInfo(value=33113, min=0, max=65535, fuzz=255, flat=4095, resolution=0)), 
        (('ABS_GAS', 9), AbsInfo(value=0, min=0, max=1023, fuzz=3, flat=63, resolution=0)), 
        (('ABS_BRAKE', 10), AbsInfo(value=0, min=0, max=1023, fuzz=3, flat=63, resolution=0)), 
        (('ABS_HAT0X', 16), AbsInfo(value=0, min=-1, max=1, fuzz=0, flat=0, resolution=0)), 
        (('ABS_HAT0Y', 17), AbsInfo(value=0, min=-1, max=1, fuzz=0, flat=0, resolution=0)), 
        (('ABS_MISC', 40), AbsInfo(value=135, min=0, max=255, fuzz=0, flat=15, resolution=0))
        ], 
    ('EV_MSC', 4): [
        ('MSC_SCAN', 4)
        ]
}


# defaults:
# left_horz = ABS_Y
# left_vert = ABS_X
# right_horz = ABS_Z
# right_vert = ABS_RZ
# l_trigger = ABS_BRAKE
# r_trigger = ABS_GAS
# pad_horz = ABS_HAT0X
# pad_vert = ABS_HAT0Y


# jstest:
# left_horz : axes 0
# left_vert : axes 1
# right_horz = axes 2
# right_vert = axes 3
# l_trigger = axes 5
# right_trigger = axes 4
# pad_horz = axes 6
# pad_vert = axes 7



Input driver version is 1.0.1
Input device ID: bus 0x5 vendor 0x45e product 0x2e0 version 0x903
Input device name: "Xbox Wireless Controller"
Supported events:
  Event type 0 (EV_SYN)
  Event type 1 (EV_KEY)
    Event code 172 (KEY_HOMEPAGE)
    Event code 304 (BTN_SOUTH)
    Event code 305 (BTN_EAST)
    Event code 307 (BTN_NORTH)
    Event code 308 (BTN_WEST)
    Event code 310 (BTN_TL)
    Event code 311 (BTN_TR)
    Event code 314 (BTN_SELECT)
    Event code 315 (BTN_START)
    Event code 317 (BTN_THUMBL)
    Event code 318 (BTN_THUMBR)
  Event type 3 (EV_ABS)
    Event code 0 (ABS_X)
      Value      0
      Min   -32768
      Max    32767
      Fuzz      32
      Flat    3072
    Event code 1 (ABS_Y)
      Value      0
      Min   -32768
      Max    32767
      Fuzz      32
      Flat    3072
    Event code 2 (ABS_Z)
      Value      0
      Min        0
      Max     1023
      Fuzz       4
    Event code 3 (ABS_RX)
      Value      0
      Min   -32768
      Max    32767
      Fuzz      32
      Flat    3072
    Event code 4 (ABS_RY)
      Value      0
      Min   -32768
      Max    32767
      Fuzz      32
      Flat    3072
    Event code 5 (ABS_RZ)
      Value      0
      Min        0
      Max     1023
      Fuzz       4
    Event code 16 (ABS_HAT0X)
      Value      0
      Min       -1
      Max        1
    Event code 17 (ABS_HAT0Y)
      Value      0
      Min       -1
      Max        1
    Event code 40 (ABS_MISC)
      Value      0
      Min    -1023
      Max     1023
      Fuzz       3
      Flat      63
  Event type 4 (EV_MSC)
    Event code 4 (MSC_SCAN)
  Event type 21 (EV_FF)
    Event code 80 (FF_RUMBLE)
    Event code 81 (FF_PERIODIC)
    Event code 88 (FF_SQUARE)
    Event code 89 (FF_TRIANGLE)
    Event code 90 (FF_SINE)
    Event code 96 (FF_GAIN)
