################################################################################
# MRS Version: 2.2.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/ch32v00X_it.c \
../User/main.c \
../User/system_ch32v00X.c 

C_DEPS += \
./User/ch32v00X_it.d \
./User/main.d \
./User/system_ch32v00X.d 

OBJS += \
./User/ch32v00X_it.o \
./User/main.o \
./User/system_ch32v00X.o 


EXPANDS += \
./User/ch32v00X_it.c.253r.expand \
./User/main.c.253r.expand \
./User/system_ch32v00X.c.253r.expand 



# Each subdirectory must supply rules for building sources it contributes
User/%.o: ../User/%.c
	@	riscv-wch-elf-gcc -march=rv32ec_zmmul_xw -mabi=ilp32e -msmall-data-limit=0 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -DSDI_PRINT=SDI_PR_OPEN -I"d:/src/ch32v/CandleLight/Debug" -I"d:/src/ch32v/CandleLight/Core" -I"d:/src/ch32v/CandleLight/User" -I"d:/src/ch32v/CandleLight/Peripheral/inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

