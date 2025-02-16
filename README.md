# Digital display device for Proteus simulation software

## Using

TestJig/Design.pdsprj : Contains an example schematic that uses TTL counters to create a video signal. Run the simulation with F12 and you should see:

![Screenshot](DigitalDisplay.jpg?raw=true "Screenshot")

Editing the properties of the component will allow the following to be configured:

	* Display width in pixels. Horizontal pixels are scaled to fit this line depending on the HSYNC signal timing.
	* Display height in pixels. Every complete HSYNC a new line is rendered, this value should therefore be the number of display lines intended to be shown.
	* Rotation. An integer to rotate the display in 90 degree steps. 0 is no rotation.
	* Write debug frames. A path or file name prefix (can include directories) to be used when writing each full frame to a BMP file, with a numeric suffix.
	* Write debug frames. Yes will cause the above prefix to be used for writing debug BMP files containing each full frame.
	* The debug output D[0..7] pins will be high when the corresponding RGB input bits are all high. Useful for setting breakpoints on specific events.
