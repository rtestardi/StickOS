#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/pic32.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/pic32.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/35980457/basic0.o ${OBJECTDIR}/_ext/35980457/cpustick.o ${OBJECTDIR}/_ext/35980457/parse2.o ${OBJECTDIR}/_ext/35980457/run2.o ${OBJECTDIR}/_ext/592584297/adc.o ${OBJECTDIR}/_ext/592584297/cdcacm.o ${OBJECTDIR}/_ext/592584297/clone.o ${OBJECTDIR}/_ext/592584297/flash.o ${OBJECTDIR}/_ext/592584297/i2c.o ${OBJECTDIR}/_ext/592584297/kbd.o ${OBJECTDIR}/_ext/592584297/lcd.o ${OBJECTDIR}/_ext/592584297/led.o ${OBJECTDIR}/_ext/592584297/pin.o ${OBJECTDIR}/_ext/592584297/printf.o ${OBJECTDIR}/_ext/592584297/qspi.o ${OBJECTDIR}/_ext/592584297/scsi.o ${OBJECTDIR}/_ext/592584297/serial.o ${OBJECTDIR}/_ext/592584297/sleep.o ${OBJECTDIR}/_ext/592584297/terminal.o ${OBJECTDIR}/_ext/592584297/text.o ${OBJECTDIR}/_ext/592584297/timer.o ${OBJECTDIR}/_ext/592584297/usb.o ${OBJECTDIR}/_ext/592584297/util.o ${OBJECTDIR}/_ext/592584297/zigflea.o ${OBJECTDIR}/_ext/1472/main.o ${OBJECTDIR}/_ext/1472/startup.o ${OBJECTDIR}/_ext/1472/vectors.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/35980457/basic0.o.d ${OBJECTDIR}/_ext/35980457/cpustick.o.d ${OBJECTDIR}/_ext/35980457/parse2.o.d ${OBJECTDIR}/_ext/35980457/run2.o.d ${OBJECTDIR}/_ext/592584297/adc.o.d ${OBJECTDIR}/_ext/592584297/cdcacm.o.d ${OBJECTDIR}/_ext/592584297/clone.o.d ${OBJECTDIR}/_ext/592584297/flash.o.d ${OBJECTDIR}/_ext/592584297/i2c.o.d ${OBJECTDIR}/_ext/592584297/kbd.o.d ${OBJECTDIR}/_ext/592584297/lcd.o.d ${OBJECTDIR}/_ext/592584297/led.o.d ${OBJECTDIR}/_ext/592584297/pin.o.d ${OBJECTDIR}/_ext/592584297/printf.o.d ${OBJECTDIR}/_ext/592584297/qspi.o.d ${OBJECTDIR}/_ext/592584297/scsi.o.d ${OBJECTDIR}/_ext/592584297/serial.o.d ${OBJECTDIR}/_ext/592584297/sleep.o.d ${OBJECTDIR}/_ext/592584297/terminal.o.d ${OBJECTDIR}/_ext/592584297/text.o.d ${OBJECTDIR}/_ext/592584297/timer.o.d ${OBJECTDIR}/_ext/592584297/usb.o.d ${OBJECTDIR}/_ext/592584297/util.o.d ${OBJECTDIR}/_ext/592584297/zigflea.o.d ${OBJECTDIR}/_ext/1472/main.o.d ${OBJECTDIR}/_ext/1472/startup.o.d ${OBJECTDIR}/_ext/1472/vectors.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/35980457/basic0.o ${OBJECTDIR}/_ext/35980457/cpustick.o ${OBJECTDIR}/_ext/35980457/parse2.o ${OBJECTDIR}/_ext/35980457/run2.o ${OBJECTDIR}/_ext/592584297/adc.o ${OBJECTDIR}/_ext/592584297/cdcacm.o ${OBJECTDIR}/_ext/592584297/clone.o ${OBJECTDIR}/_ext/592584297/flash.o ${OBJECTDIR}/_ext/592584297/i2c.o ${OBJECTDIR}/_ext/592584297/kbd.o ${OBJECTDIR}/_ext/592584297/lcd.o ${OBJECTDIR}/_ext/592584297/led.o ${OBJECTDIR}/_ext/592584297/pin.o ${OBJECTDIR}/_ext/592584297/printf.o ${OBJECTDIR}/_ext/592584297/qspi.o ${OBJECTDIR}/_ext/592584297/scsi.o ${OBJECTDIR}/_ext/592584297/serial.o ${OBJECTDIR}/_ext/592584297/sleep.o ${OBJECTDIR}/_ext/592584297/terminal.o ${OBJECTDIR}/_ext/592584297/text.o ${OBJECTDIR}/_ext/592584297/timer.o ${OBJECTDIR}/_ext/592584297/usb.o ${OBJECTDIR}/_ext/592584297/util.o ${OBJECTDIR}/_ext/592584297/zigflea.o ${OBJECTDIR}/_ext/1472/main.o ${OBJECTDIR}/_ext/1472/startup.o ${OBJECTDIR}/_ext/1472/vectors.o


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/pic32.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=32MX795F512L
MP_LINKER_FILE_OPTION=
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/35980457/basic0.o: ../cpustick/basic0.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/35980457 
	@${RM} ${OBJECTDIR}/_ext/35980457/basic0.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/35980457/basic0.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/35980457/basic0.o.d" -o ${OBJECTDIR}/_ext/35980457/basic0.o ../cpustick/basic0.c   -fno-builtin
	
${OBJECTDIR}/_ext/35980457/cpustick.o: ../cpustick/cpustick.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/35980457 
	@${RM} ${OBJECTDIR}/_ext/35980457/cpustick.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/35980457/cpustick.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/35980457/cpustick.o.d" -o ${OBJECTDIR}/_ext/35980457/cpustick.o ../cpustick/cpustick.c   -fno-builtin
	
${OBJECTDIR}/_ext/35980457/parse2.o: ../cpustick/parse2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/35980457 
	@${RM} ${OBJECTDIR}/_ext/35980457/parse2.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/35980457/parse2.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/35980457/parse2.o.d" -o ${OBJECTDIR}/_ext/35980457/parse2.o ../cpustick/parse2.c   -fno-builtin
	
${OBJECTDIR}/_ext/35980457/run2.o: ../cpustick/run2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/35980457 
	@${RM} ${OBJECTDIR}/_ext/35980457/run2.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/35980457/run2.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/35980457/run2.o.d" -o ${OBJECTDIR}/_ext/35980457/run2.o ../cpustick/run2.c   -fno-builtin
	
${OBJECTDIR}/_ext/592584297/adc.o: ../sources/adc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/adc.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/592584297/adc.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/592584297/adc.o.d" -o ${OBJECTDIR}/_ext/592584297/adc.o ../sources/adc.c   -fno-builtin
	
${OBJECTDIR}/_ext/592584297/cdcacm.o: ../sources/cdcacm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/cdcacm.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/592584297/cdcacm.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/592584297/cdcacm.o.d" -o ${OBJECTDIR}/_ext/592584297/cdcacm.o ../sources/cdcacm.c   -fno-builtin
	
${OBJECTDIR}/_ext/592584297/clone.o: ../sources/clone.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/clone.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/592584297/clone.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/592584297/clone.o.d" -o ${OBJECTDIR}/_ext/592584297/clone.o ../sources/clone.c   -fno-builtin
	
${OBJECTDIR}/_ext/592584297/flash.o: ../sources/flash.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/flash.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/592584297/flash.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/592584297/flash.o.d" -o ${OBJECTDIR}/_ext/592584297/flash.o ../sources/flash.c   -fno-builtin
	
${OBJECTDIR}/_ext/592584297/i2c.o: ../sources/i2c.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/i2c.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/592584297/i2c.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/592584297/i2c.o.d" -o ${OBJECTDIR}/_ext/592584297/i2c.o ../sources/i2c.c   -fno-builtin
	
${OBJECTDIR}/_ext/592584297/kbd.o: ../sources/kbd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/kbd.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/592584297/kbd.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/592584297/kbd.o.d" -o ${OBJECTDIR}/_ext/592584297/kbd.o ../sources/kbd.c   -fno-builtin
	
${OBJECTDIR}/_ext/592584297/lcd.o: ../sources/lcd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/lcd.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/592584297/lcd.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/592584297/lcd.o.d" -o ${OBJECTDIR}/_ext/592584297/lcd.o ../sources/lcd.c   -fno-builtin
	
${OBJECTDIR}/_ext/592584297/led.o: ../sources/led.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/led.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/592584297/led.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/592584297/led.o.d" -o ${OBJECTDIR}/_ext/592584297/led.o ../sources/led.c   -fno-builtin
	
${OBJECTDIR}/_ext/592584297/pin.o: ../sources/pin.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/pin.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/592584297/pin.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/592584297/pin.o.d" -o ${OBJECTDIR}/_ext/592584297/pin.o ../sources/pin.c   -fno-builtin
	
${OBJECTDIR}/_ext/592584297/printf.o: ../sources/printf.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/printf.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/592584297/printf.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/592584297/printf.o.d" -o ${OBJECTDIR}/_ext/592584297/printf.o ../sources/printf.c   -fno-builtin
	
${OBJECTDIR}/_ext/592584297/qspi.o: ../sources/qspi.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/qspi.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/592584297/qspi.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/592584297/qspi.o.d" -o ${OBJECTDIR}/_ext/592584297/qspi.o ../sources/qspi.c   -fno-builtin
	
${OBJECTDIR}/_ext/592584297/scsi.o: ../sources/scsi.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/scsi.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/592584297/scsi.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/592584297/scsi.o.d" -o ${OBJECTDIR}/_ext/592584297/scsi.o ../sources/scsi.c   -fno-builtin
	
${OBJECTDIR}/_ext/592584297/serial.o: ../sources/serial.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/serial.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/592584297/serial.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/592584297/serial.o.d" -o ${OBJECTDIR}/_ext/592584297/serial.o ../sources/serial.c   -fno-builtin
	
${OBJECTDIR}/_ext/592584297/sleep.o: ../sources/sleep.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/sleep.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/592584297/sleep.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/592584297/sleep.o.d" -o ${OBJECTDIR}/_ext/592584297/sleep.o ../sources/sleep.c   -fno-builtin
	
${OBJECTDIR}/_ext/592584297/terminal.o: ../sources/terminal.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/terminal.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/592584297/terminal.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/592584297/terminal.o.d" -o ${OBJECTDIR}/_ext/592584297/terminal.o ../sources/terminal.c   -fno-builtin
	
${OBJECTDIR}/_ext/592584297/text.o: ../sources/text.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/text.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/592584297/text.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/592584297/text.o.d" -o ${OBJECTDIR}/_ext/592584297/text.o ../sources/text.c   -fno-builtin
	
${OBJECTDIR}/_ext/592584297/timer.o: ../sources/timer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/timer.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/592584297/timer.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/592584297/timer.o.d" -o ${OBJECTDIR}/_ext/592584297/timer.o ../sources/timer.c   -fno-builtin
	
${OBJECTDIR}/_ext/592584297/usb.o: ../sources/usb.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/usb.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/592584297/usb.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/592584297/usb.o.d" -o ${OBJECTDIR}/_ext/592584297/usb.o ../sources/usb.c   -fno-builtin
	
${OBJECTDIR}/_ext/592584297/util.o: ../sources/util.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/util.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/592584297/util.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/592584297/util.o.d" -o ${OBJECTDIR}/_ext/592584297/util.o ../sources/util.c   -fno-builtin
	
${OBJECTDIR}/_ext/592584297/zigflea.o: ../sources/zigflea.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/zigflea.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/592584297/zigflea.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/592584297/zigflea.o.d" -o ${OBJECTDIR}/_ext/592584297/zigflea.o ../sources/zigflea.c   -fno-builtin
	
${OBJECTDIR}/_ext/1472/main.o: ../main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/main.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/1472/main.o.d" -o ${OBJECTDIR}/_ext/1472/main.o ../main.c   -fno-builtin
	
${OBJECTDIR}/_ext/1472/startup.o: ../startup.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/startup.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/startup.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/1472/startup.o.d" -o ${OBJECTDIR}/_ext/1472/startup.o ../startup.c   -fno-builtin
	
${OBJECTDIR}/_ext/1472/vectors.o: ../vectors.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/vectors.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/vectors.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/1472/vectors.o.d" -o ${OBJECTDIR}/_ext/1472/vectors.o ../vectors.c   -fno-builtin
	
else
${OBJECTDIR}/_ext/35980457/basic0.o: ../cpustick/basic0.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/35980457 
	@${RM} ${OBJECTDIR}/_ext/35980457/basic0.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/35980457/basic0.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/35980457/basic0.o.d" -o ${OBJECTDIR}/_ext/35980457/basic0.o ../cpustick/basic0.c   -fno-builtin
	
${OBJECTDIR}/_ext/35980457/cpustick.o: ../cpustick/cpustick.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/35980457 
	@${RM} ${OBJECTDIR}/_ext/35980457/cpustick.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/35980457/cpustick.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/35980457/cpustick.o.d" -o ${OBJECTDIR}/_ext/35980457/cpustick.o ../cpustick/cpustick.c   -fno-builtin
	
${OBJECTDIR}/_ext/35980457/parse2.o: ../cpustick/parse2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/35980457 
	@${RM} ${OBJECTDIR}/_ext/35980457/parse2.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/35980457/parse2.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/35980457/parse2.o.d" -o ${OBJECTDIR}/_ext/35980457/parse2.o ../cpustick/parse2.c   -fno-builtin
	
${OBJECTDIR}/_ext/35980457/run2.o: ../cpustick/run2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/35980457 
	@${RM} ${OBJECTDIR}/_ext/35980457/run2.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/35980457/run2.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/35980457/run2.o.d" -o ${OBJECTDIR}/_ext/35980457/run2.o ../cpustick/run2.c   -fno-builtin
	
${OBJECTDIR}/_ext/592584297/adc.o: ../sources/adc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/adc.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/592584297/adc.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/592584297/adc.o.d" -o ${OBJECTDIR}/_ext/592584297/adc.o ../sources/adc.c   -fno-builtin
	
${OBJECTDIR}/_ext/592584297/cdcacm.o: ../sources/cdcacm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/cdcacm.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/592584297/cdcacm.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/592584297/cdcacm.o.d" -o ${OBJECTDIR}/_ext/592584297/cdcacm.o ../sources/cdcacm.c   -fno-builtin
	
${OBJECTDIR}/_ext/592584297/clone.o: ../sources/clone.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/clone.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/592584297/clone.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/592584297/clone.o.d" -o ${OBJECTDIR}/_ext/592584297/clone.o ../sources/clone.c   -fno-builtin
	
${OBJECTDIR}/_ext/592584297/flash.o: ../sources/flash.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/flash.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/592584297/flash.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/592584297/flash.o.d" -o ${OBJECTDIR}/_ext/592584297/flash.o ../sources/flash.c   -fno-builtin
	
${OBJECTDIR}/_ext/592584297/i2c.o: ../sources/i2c.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/i2c.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/592584297/i2c.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/592584297/i2c.o.d" -o ${OBJECTDIR}/_ext/592584297/i2c.o ../sources/i2c.c   -fno-builtin
	
${OBJECTDIR}/_ext/592584297/kbd.o: ../sources/kbd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/kbd.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/592584297/kbd.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/592584297/kbd.o.d" -o ${OBJECTDIR}/_ext/592584297/kbd.o ../sources/kbd.c   -fno-builtin
	
${OBJECTDIR}/_ext/592584297/lcd.o: ../sources/lcd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/lcd.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/592584297/lcd.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/592584297/lcd.o.d" -o ${OBJECTDIR}/_ext/592584297/lcd.o ../sources/lcd.c   -fno-builtin
	
${OBJECTDIR}/_ext/592584297/led.o: ../sources/led.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/led.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/592584297/led.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/592584297/led.o.d" -o ${OBJECTDIR}/_ext/592584297/led.o ../sources/led.c   -fno-builtin
	
${OBJECTDIR}/_ext/592584297/pin.o: ../sources/pin.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/pin.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/592584297/pin.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/592584297/pin.o.d" -o ${OBJECTDIR}/_ext/592584297/pin.o ../sources/pin.c   -fno-builtin
	
${OBJECTDIR}/_ext/592584297/printf.o: ../sources/printf.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/printf.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/592584297/printf.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/592584297/printf.o.d" -o ${OBJECTDIR}/_ext/592584297/printf.o ../sources/printf.c   -fno-builtin
	
${OBJECTDIR}/_ext/592584297/qspi.o: ../sources/qspi.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/qspi.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/592584297/qspi.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/592584297/qspi.o.d" -o ${OBJECTDIR}/_ext/592584297/qspi.o ../sources/qspi.c   -fno-builtin
	
${OBJECTDIR}/_ext/592584297/scsi.o: ../sources/scsi.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/scsi.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/592584297/scsi.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/592584297/scsi.o.d" -o ${OBJECTDIR}/_ext/592584297/scsi.o ../sources/scsi.c   -fno-builtin
	
${OBJECTDIR}/_ext/592584297/serial.o: ../sources/serial.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/serial.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/592584297/serial.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/592584297/serial.o.d" -o ${OBJECTDIR}/_ext/592584297/serial.o ../sources/serial.c   -fno-builtin
	
${OBJECTDIR}/_ext/592584297/sleep.o: ../sources/sleep.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/sleep.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/592584297/sleep.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/592584297/sleep.o.d" -o ${OBJECTDIR}/_ext/592584297/sleep.o ../sources/sleep.c   -fno-builtin
	
${OBJECTDIR}/_ext/592584297/terminal.o: ../sources/terminal.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/terminal.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/592584297/terminal.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/592584297/terminal.o.d" -o ${OBJECTDIR}/_ext/592584297/terminal.o ../sources/terminal.c   -fno-builtin
	
${OBJECTDIR}/_ext/592584297/text.o: ../sources/text.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/text.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/592584297/text.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/592584297/text.o.d" -o ${OBJECTDIR}/_ext/592584297/text.o ../sources/text.c   -fno-builtin
	
${OBJECTDIR}/_ext/592584297/timer.o: ../sources/timer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/timer.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/592584297/timer.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/592584297/timer.o.d" -o ${OBJECTDIR}/_ext/592584297/timer.o ../sources/timer.c   -fno-builtin
	
${OBJECTDIR}/_ext/592584297/usb.o: ../sources/usb.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/usb.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/592584297/usb.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/592584297/usb.o.d" -o ${OBJECTDIR}/_ext/592584297/usb.o ../sources/usb.c   -fno-builtin
	
${OBJECTDIR}/_ext/592584297/util.o: ../sources/util.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/util.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/592584297/util.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/592584297/util.o.d" -o ${OBJECTDIR}/_ext/592584297/util.o ../sources/util.c   -fno-builtin
	
${OBJECTDIR}/_ext/592584297/zigflea.o: ../sources/zigflea.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/zigflea.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/592584297/zigflea.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/592584297/zigflea.o.d" -o ${OBJECTDIR}/_ext/592584297/zigflea.o ../sources/zigflea.c   -fno-builtin
	
${OBJECTDIR}/_ext/1472/main.o: ../main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/main.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/1472/main.o.d" -o ${OBJECTDIR}/_ext/1472/main.o ../main.c   -fno-builtin
	
${OBJECTDIR}/_ext/1472/startup.o: ../startup.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/startup.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/startup.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/1472/startup.o.d" -o ${OBJECTDIR}/_ext/1472/startup.o ../startup.c   -fno-builtin
	
${OBJECTDIR}/_ext/1472/vectors.o: ../vectors.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/vectors.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/vectors.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/1472/vectors.o.d" -o ${OBJECTDIR}/_ext/1472/vectors.o ../vectors.c   -fno-builtin
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/pic32.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mdebugger -D__MPLAB_DEBUGGER_PIC32MXSK=1 -mprocessor=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/pic32.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}        -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__MPLAB_DEBUG=1,--defsym=__ICD2RAM=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PIC32MXSK=1,--defsym=_min_heap_size=0,--defsym=_min_stack_size=3072,-L"../../../../Program Files/Microchip/MPLAB C32/lib",-L"../../../../Program Files/Microchip/MPLAB C32/pic32mx/lib",-Map="$(BINDIR_)$(TARGETBASE).map",../pic32stickos.X/dist/default/production/pic32stickos.x.a -Wl,--script=../elf32pic32mx.old.ld 
else
dist/${CND_CONF}/${IMAGE_TYPE}/pic32.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mprocessor=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/pic32.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}        -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=_min_heap_size=0,--defsym=_min_stack_size=3072,-L"../../../../Program Files/Microchip/MPLAB C32/lib",-L"../../../../Program Files/Microchip/MPLAB C32/pic32mx/lib",-Map="$(BINDIR_)$(TARGETBASE).map",../pic32stickos.X/dist/default/production/pic32stickos.x.a -Wl,--script=../elf32pic32mx.old.ld
	${MP_CC_DIR}\\pic32-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/pic32.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
