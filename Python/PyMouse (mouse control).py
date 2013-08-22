# import pymouse module
from pymouse import PyMouse
m = PyMouse()
import serial
import time

# initialize and connect to serial port
ser = serial.Serial('/dev/ttyACM0', 9600)

# set current position of mouse on the screen as reference
reference = m.position()
reference_x=reference[0]
reference_y=reference[1]

#initialize values of new_x and new_y
new_x=reference[0]
new_y=reference[1]

# mapping table for HEX codes received from arduino
# cursor movement 
move_right1 = list('A94\r\n')
move_right2 = list('A95\r\n')
move_left1 = list('B94\r\n')
move_left2 = list('B95\r\n')
move_up1 = list('C94\r\n')
move_up2 = list('C95\r\n')
move_down1 = list('D94\r\n')
move_down2 = list('D95\r\n')
moveUpRight = list('E94\r\n')
moveUpLeft = list('E95\r\n')
moveDownRight = list('F94\r\n')
moveDownLeft = list('F95\r\n')

# mouse click events
leftClick = list('A92\r\n')
leftRelease = list('A93\r\n')
select2 = list('A90\r\n')

# set limits upto which the cursor can move on the screen
screenSize = m.screen_size()
xUpper = screenSize[0]-11
xLower = 10
yUpper = screenSize[1] - 9
yLower = 8

while True:
    
    input_hex=list(ser.readline()) # read the incoming value from serial port

    # refresh the reference values
    reference = m.position()
    reference_x=reference[0]
    reference_y=reference[1]

    # cursor movements
    if ( input_hex == move_right1):
        print 'right1'
        
        if(reference_x<xUpper):
            
            for x in range (5):
              reference_x += 1
              m.move(reference_x,reference_y)
              time.sleep(0.020)

    elif ( input_hex == move_right2):
        print 'right2'
        
        if(reference_x<xUpper):
            
            for x in range (20):
              reference_x += 1
              m.move(reference_x,reference_y)
              time.sleep(0.005)
            

    elif ( input_hex == move_left1):
        print 'left1'
        
        if(reference_x>xLower):
           
            for x in range (5):
                reference_x -= 1
                m.move(reference_x,reference_y)
                time.sleep(0.020)

    elif ( input_hex == move_left2):
        print 'left2'
        
        if(reference_x>xLower):
           
            for x in range (20):
                reference_x -= 1
                m.move(reference_x,reference_y)
                time.sleep(0.005)

    elif ( input_hex == move_up1):
        print 'up1'
        
        if(reference_y>yLower):
            
            for x in range (5):
                reference_y -= 1
                m.move(reference_x,reference_y)
                time.sleep(0.020)

    elif ( input_hex == move_up2):
        print 'up2'
        
        if(reference_y>yLower):
           
            for x in range (15):
                reference_y -= 1
                m.move(reference_x,reference_y)
                time.sleep(0.005)

    elif ( input_hex == move_down1):
        print 'down1'
        
        if(reference_y<yUpper):
            
            for x in range (5):
                reference_y += 1
                m.move(reference_x,reference_y)
                time.sleep(0.020)

    elif ( input_hex == move_down2):
        print 'down2'
        
        if(reference_y<yUpper):
            
            for x in range (15):
                reference_y += 1
                m.move(reference_x,reference_y)
                time.sleep(0.005)

    elif ( input_hex == moveUpRight):
        print 'up-right'
        
        if(reference_y>yLower and reference_x<xUpper):
            
            for x in range (15):
                reference_y -= 1
                reference_x += 1
                m.move(reference_x,reference_y)
                time.sleep(0.005)

    elif (input_hex == moveUpLeft):
        print 'up-left'

        if(reference_y>yLower and reference_x>xLower):
            
            for x in range(15):
                reference_y -= 1
                reference_x -= 1
                m.move(reference_x,reference_y)
                time.sleep(0.005)

    elif (input_hex == moveDownRight):
        print 'down-right'

        if(reference_y<yUpper and reference_x<xUpper):
            
            for x in range(15):
                reference_y += 1
                reference_x += 1
                m.move(reference_x,reference_y)
                time.sleep(0.005)

    elif (input_hex == moveDownLeft):
        print 'down-left'

        if(reference_y<yUpper and reference_x>xLower):
            
            for x in range(15):
                reference_y += 1
                reference_x -= 1
                m.move(reference_x,reference_y)
                time.sleep(0.005)
                
                
    # mouse click events
    elif ( input_hex == leftClick):
        m.press(reference_x,reference_y,1)

    elif ( input_hex == leftRelease):
        m.release(reference_x,reference_y,1)

    elif ( input_hex == select2):
        m.click(reference_x,reference_y,1)
        m.click(reference_x,reference_y,1)
  
    
