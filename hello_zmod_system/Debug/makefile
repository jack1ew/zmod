#
# This file was created by Xilinx Vitis tool.
#


#
# tools
#

RM = rm -f
RMDIR = rm -rf



#
# variables
#

SDCARD_GEN = sdcard_gen
GENERATE_BIF = ::scw::generate_bif

XPFM_PATH = D:/Vitis/eclypse_0/export/eclypse_0/eclypse_0.xpfm
SYS_CONFIG = eclypse_0
DOMAINS = standalone_domain
BIF_PATH = D:/Vitis/hello_zmod_system/Debug/system.bif

BITSTREAM_PATH = D:/Vitis/hello_zmod/_ide/bitstream/design_1_wrapper.bit
SD_FILES = --elf D:/Vitis/hello_zmod/Debug/hello_zmod.elf,ps7_cortexa9_0

#
# primary build targets
#

package:
	-@echo Generating bif file for the system project
	generate_system_bif.bat 14866 ${XPFM_PATH} ${DOMAINS} ${BIF_PATH}
	${SDCARD_GEN} --xpfm ${XPFM_PATH} --sys_config ${SYS_CONFIG} --bif ${BIF_PATH} --bitstream ${BITSTREAM_PATH} ${SD_FILES}

clean:
	$(RMDIR) ./_sds
	${RMDIR} package
	${RM} system.bif

.PHONY: all package clean

all: package


