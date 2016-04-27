################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../cliente.c \
../servidor.c \
../socketComms.c \
../umc.c 

OBJS += \
./cliente.o \
./servidor.o \
./socketComms.o \
./umc.o 

C_DEPS += \
./cliente.d \
./servidor.d \
./socketComms.d \
./umc.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/home/utnso/tp-2016-1c-Hellfish-Group/nucleo -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

umc.o: ../umc.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2016-1c-Hellfish-Group/sources comunes" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"umc.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


