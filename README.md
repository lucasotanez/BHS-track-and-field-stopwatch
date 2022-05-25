# track-and-field-stopwatch

This is the Arduino code for the jumbo stopwatch I made for my high school's track and field team to use during meets. This jumbo stopwatch replaced the handheld
stopwatch that was being used prior. This new jumbo stopwatch allows the crowd to view a runner's time from anywhere in the stands. The stopwatch also utilizes
a foot pressure sensor and a laser sensor for starting and stopping the runner's time.

## materials

- 6 7-segment displays (6.5 inch) https://www.sparkfun.com/products/8530 
- 6 large digit drivers https://www.sparkfun.com/products/13279
- Arduino UNO or SparkFun Redboard https://www.sparkfun.com/products/13975
- breadboard (optional)
- jumper wires, resistors, LEDs
- assorted M4 screws
- a box or frame to mount the displays

## instructions

- lasercut using the included CAD files or manually cut a box for the displays
- daisy chain and solder the large digit drivers for all 6 displays (see image)
- connect all wires to the correct GPIO pins on the board, driver, and breadboard using the pinout diagram
