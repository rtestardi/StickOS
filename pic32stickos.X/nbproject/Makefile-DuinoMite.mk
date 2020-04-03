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
ifeq "$(wildcard nbproject/Makefile-local-DuinoMite.mk)" "nbproject/Makefile-local-DuinoMite.mk"
include nbproject/Makefile-local-DuinoMite.mk
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=DuinoMite
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=a
DEBUGGABLE_SUFFIX=
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/pic32stickos.X.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=a
DEBUGGABLE_SUFFIX=
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/pic32stickos.X.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/724208933/basic.o ${OBJECTDIR}/_ext/555720668/block.o ${OBJECTDIR}/_ext/724208933/code.o ${OBJECTDIR}/_ext/555720668/fat32.o ${OBJECTDIR}/_ext/724208933/parse.o ${OBJECTDIR}/_ext/724208933/run.o ${OBJECTDIR}/_ext/724208933/vars.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/724208933/basic.o.d ${OBJECTDIR}/_ext/555720668/block.o.d ${OBJECTDIR}/_ext/724208933/code.o.d ${OBJECTDIR}/_ext/555720668/fat32.o.d ${OBJECTDIR}/_ext/724208933/parse.o.d ${OBJECTDIR}/_ext/724208933/run.o.d ${OBJECTDIR}/_ext/724208933/vars.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/724208933/basic.o ${OBJECTDIR}/_ext/555720668/block.o ${OBJECTDIR}/_ext/724208933/code.o ${OBJECTDIR}/_ext/555720668/fat32.o ${OBJECTDIR}/_ext/724208933/parse.o ${OBJECTDIR}/_ext/724208933/run.o ${OBJECTDIR}/_ext/724208933/vars.o


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
	${MAKE}  -f nbproject/Makefile-DuinoMite.mk dist/${CND_CONF}/${IMAGE_TYPE}/pic32stickos.X.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=32MX460F512L
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
${OBJECTDIR}/_ext/724208933/basic.o: ../stickos/basic.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/724208933 
	@${RM} ${OBJECTDIR}/_ext/724208933/basic.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/724208933/basic.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DDUINOMITE=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/724208933/basic.o.d" -o ${OBJECTDIR}/_ext/724208933/basic.o ../stickos/basic.c   -fno-builtin
	
${OBJECTDIR}/_ext/555720668/block.o: ../pict-o-crypt/block.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/555720668 
	@${RM} ${OBJECTDIR}/_ext/555720668/block.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/555720668/block.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DDUINOMITE=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/555720668/block.o.d" -o ${OBJECTDIR}/_ext/555720668/block.o ../pict-o-crypt/block.c   -fno-builtin
	
${OBJECTDIR}/_ext/724208933/code.o: ../stickos/code.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/724208933 
	@${RM} ${OBJECTDIR}/_ext/724208933/code.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/724208933/code.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DDUINOMITE=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/724208933/code.o.d" -o ${OBJECTDIR}/_ext/724208933/code.o ../stickos/code.c   -fno-builtin
	
${OBJECTDIR}/_ext/555720668/fat32.o: ../pict-o-crypt/fat32.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/555720668 
	@${RM} ${OBJECTDIR}/_ext/555720668/fat32.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/555720668/fat32.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DDUINOMITE=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/555720668/fat32.o.d" -o ${OBJECTDIR}/_ext/555720668/fat32.o ../pict-o-crypt/fat32.c   -fno-builtin
	
${OBJECTDIR}/_ext/724208933/parse.o: ../stickos/parse.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/724208933 
	@${RM} ${OBJECTDIR}/_ext/724208933/parse.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/724208933/parse.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DDUINOMITE=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/724208933/parse.o.d" -o ${OBJECTDIR}/_ext/724208933/parse.o ../stickos/parse.c   -fno-builtin
	
${OBJECTDIR}/_ext/724208933/run.o: ../stickos/run.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/724208933 
	@${RM} ${OBJECTDIR}/_ext/724208933/run.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/724208933/run.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DDUINOMITE=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/724208933/run.o.d" -o ${OBJECTDIR}/_ext/724208933/run.o ../stickos/run.c   -fno-builtin
	
${OBJECTDIR}/_ext/724208933/vars.o: ../stickos/vars.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/724208933 
	@${RM} ${OBJECTDIR}/_ext/724208933/vars.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/724208933/vars.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DDUINOMITE=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/724208933/vars.o.d" -o ${OBJECTDIR}/_ext/724208933/vars.o ../stickos/vars.c   -fno-builtin
	
else
${OBJECTDIR}/_ext/724208933/basic.o: ../stickos/basic.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/724208933 
	@${RM} ${OBJECTDIR}/_ext/724208933/basic.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/724208933/basic.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DDUINOMITE=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/724208933/basic.o.d" -o ${OBJECTDIR}/_ext/724208933/basic.o ../stickos/basic.c   -fno-builtin
	
${OBJECTDIR}/_ext/555720668/block.o: ../pict-o-crypt/block.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/555720668 
	@${RM} ${OBJECTDIR}/_ext/555720668/block.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/555720668/block.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DDUINOMITE=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/555720668/block.o.d" -o ${OBJECTDIR}/_ext/555720668/block.o ../pict-o-crypt/block.c   -fno-builtin
	
${OBJECTDIR}/_ext/724208933/code.o: ../stickos/code.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/724208933 
	@${RM} ${OBJECTDIR}/_ext/724208933/code.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/724208933/code.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DDUINOMITE=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/724208933/code.o.d" -o ${OBJECTDIR}/_ext/724208933/code.o ../stickos/code.c   -fno-builtin
	
${OBJECTDIR}/_ext/555720668/fat32.o: ../pict-o-crypt/fat32.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/555720668 
	@${RM} ${OBJECTDIR}/_ext/555720668/fat32.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/555720668/fat32.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DDUINOMITE=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/555720668/fat32.o.d" -o ${OBJECTDIR}/_ext/555720668/fat32.o ../pict-o-crypt/fat32.c   -fno-builtin
	
${OBJECTDIR}/_ext/724208933/parse.o: ../stickos/parse.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/724208933 
	@${RM} ${OBJECTDIR}/_ext/724208933/parse.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/724208933/parse.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DDUINOMITE=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/724208933/parse.o.d" -o ${OBJECTDIR}/_ext/724208933/parse.o ../stickos/parse.c   -fno-builtin
	
${OBJECTDIR}/_ext/724208933/run.o: ../stickos/run.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/724208933 
	@${RM} ${OBJECTDIR}/_ext/724208933/run.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/724208933/run.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DDUINOMITE=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/724208933/run.o.d" -o ${OBJECTDIR}/_ext/724208933/run.o ../stickos/run.c   -fno-builtin
	
${OBJECTDIR}/_ext/724208933/vars.o: ../stickos/vars.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/724208933 
	@${RM} ${OBJECTDIR}/_ext/724208933/vars.o.d 
	@${FIXDEPS} "${OBJECTDIR}/_ext/724208933/vars.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DDUINOMITE=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -MMD -MF "${OBJECTDIR}/_ext/724208933/vars.o.d" -o ${OBJECTDIR}/_ext/724208933/vars.o ../stickos/vars.c   -fno-builtin
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: archive
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/pic32stickos.X.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_AR} $(MP_EXTRA_AR_PRE) r dist/${CND_CONF}/${IMAGE_TYPE}/pic32stickos.X.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}     
else
dist/${CND_CONF}/${IMAGE_TYPE}/pic32stickos.X.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_AR} $(MP_EXTRA_AR_PRE) r dist/${CND_CONF}/${IMAGE_TYPE}/pic32stickos.X.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}     
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/DuinoMite
	${RM} -r dist/DuinoMite

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
