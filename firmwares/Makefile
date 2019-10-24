all:

fetch_%:
	wget -q "http://firmwares.bonoboswd.com/uc/v1.0/nuttx-ocd.bin" -O nuttx-ocd.bin
	wget -q "http://firmwares.bonoboswd.com/fpga/v1.0/build.swd/$*/gateware/top.bin" -O top.bin

flash_fpga:
	openocd -f openocd-fpga.cfg

flash_uc:
	openocd -f openocd-stm-ocd.cfg
