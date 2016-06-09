################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../cliente.c \
../consola.c \
../interfazNucleo.c \
../socketComms.c 

OBJS += \
./cliente.o \
./consola.o \
./interfazNucleo.o \
./socketComms.o 

C_DEPS += \
./cliente.d \
./consola.d \
./interfazNucleo.d \
./socketComms.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/home/utnso/tp-2016-1c-Hellfish-Group/nucleo -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


