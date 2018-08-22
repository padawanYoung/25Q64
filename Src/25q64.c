#include "main.h"
#include "stm32f4xx_hal.h"

#include "stdlib.h"
#include "stdbool.h"
#include "25q64_lib.h"

extern SPI_HandleTypeDef hspi1;

void Memory_Select_Slave(bool action) {

	if (action) {
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
	} else {
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
	}
	return;
}

void Memory_Write_Permission(bool instruction) {
	uint8_t command;
	Memory_Select_Slave(true);
	if (instruction) {
		command = WriteEnable;
		HAL_SPI_Transmit(&hspi1, (uint8_t *) &command, 1, 5);
	} else {
		command = WriteDisable;
		HAL_SPI_Transmit(&hspi1, (uint8_t *) &command, 1, 5);
	}
	Memory_Select_Slave(false);
	return;
}

uint8_t Memory_ReadStatusRegister(uint8_t number) {
	uint8_t status;
	uint8_t callback;
	Memory_Select_Slave(true);
	if (number == 1) {
		status = ReadStatusRegister1;
		HAL_SPI_Transmit(&hspi1, (uint8_t *) &status, 1, 5);
	}
	if (number == 2) {
		status = ReadStatusRegister2;
		HAL_SPI_Transmit(&hspi1, (uint8_t *) &status, 1, 5);
	}
	if (number == 3) {
		status = ReadStatusRegister3;
		HAL_SPI_Transmit(&hspi1, (uint8_t *) &status, 1, 5);
	}
	HAL_SPI_Receive(&hspi1, &callback, 1, 5);
	Memory_Select_Slave(false);
	return callback;
}

void Memory_WriteStatusRegister(uint8_t number, uint8_t value) {
	uint8_t status;
	Memory_Write_Permission(true);
	Memory_Select_Slave(true);
	if (number == 1) {
		status = WriteStatusRegister1;
		HAL_SPI_Transmit(&hspi1, (uint8_t *) &status, 1, 5);
	}
	if (number == 2) {
		status = WriteStatusRegister2;
		HAL_SPI_Transmit(&hspi1, (uint8_t *) &status, 1, 5);
	}
	if (number == 3) {
		status = WriteStatusRegister3;
		HAL_SPI_Transmit(&hspi1, (uint8_t *) &status, 1, 5);
	}
	HAL_SPI_Transmit(&hspi1, (uint8_t *) &value, 1, 5);
	Memory_Select_Slave(false);
	return;
}

void Memory_BusyDelay() {
	while (Memory_ReadStatusRegister(1) & 0x01)
		;
	return;
}

void Memory_ChipErase() {
	Memory_Write_Permission(true);
	Memory_Select_Slave(true);
	uint8_t command = ChipErase;
	HAL_SPI_Transmit(&hspi1, (uint8_t *) &command, 1, 5);
	Memory_Select_Slave(false);
	Memory_BusyDelay();
	return;
}

void Memory_BlockErase_4kb(uint32_t address) {
	Memory m;
	uint8_t command;
	command = SectorErase_4kb;
	m.addr32 = address;
	Memory_Write_Permission(true);
	Memory_Select_Slave(true);
	HAL_SPI_Transmit(&hspi1, (uint8_t *) &command, 1, 5);
	HAL_SPI_Transmit(&hspi1, (uint8_t *) &m.addr8[2], 1, 5);
	HAL_SPI_Transmit(&hspi1, (uint8_t *) &m.addr8[1], 1, 5);
	HAL_SPI_Transmit(&hspi1, (uint8_t *) &m.addr8[0], 1, 5);
	Memory_Select_Slave(false);
	Memory_Write_Permission(false);
	return;
}

uint8_t Memory_ReadDataBytes(uint32_t address) {
	uint8_t command = ReadData;
	uint8_t data;
	Memory m;
	m.addr32 = address;

	Memory_Select_Slave(true);

	HAL_SPI_Transmit(&hspi1, (uint8_t *) &command, 1, 5);
	HAL_SPI_Transmit(&hspi1, (uint8_t *) &m.addr8[2], 1, 5);
	HAL_SPI_Transmit(&hspi1, (uint8_t *) &m.addr8[1], 1, 5);
	HAL_SPI_Transmit(&hspi1, (uint8_t *) &m.addr8[0], 1, 5);
	HAL_SPI_Receive(&hspi1, &data, 1, 5);

	Memory_Select_Slave(false);
	return data;
}

void Memory_PowerDown(bool state) {
	uint8_t command;
	Memory_Select_Slave(true);
	if (state) {
		command = PowerDown;
		HAL_SPI_Transmit(&hspi1, (uint8_t *) &command, 1, 5);
	} else {
		command = ReleasePowerDown;
		HAL_SPI_Transmit(&hspi1, (uint8_t *) &command, 1, 5);
		command = 0xff;
		HAL_SPI_Transmit(&hspi1, (uint8_t *) &command, 1, 5);
		HAL_SPI_Transmit(&hspi1, (uint8_t *) &command, 1, 5);
		HAL_SPI_Transmit(&hspi1, (uint8_t *) &command, 1, 5);
		HAL_SPI_Transmit(&hspi1, (uint8_t *) &command, 1, 5);
	}
	Memory_Select_Slave(false);
	return;
}

void Memory_Reset() {
	uint8_t command;
	Memory_Select_Slave(true);
	command = EnableReset;
	HAL_SPI_Transmit(&hspi1, (uint8_t *) &command, 1, 5);
	Memory_Select_Slave(false);
	Memory_Select_Slave(true);
	command = Reset;
	HAL_SPI_Transmit(&hspi1, (uint8_t *) &command, 1, 5);
	Memory_Select_Slave(false);
}

void Memory_WriteData(DataStructure * data) {
	uint8_t command = PageProgram;
	Memory_Write_Permission(true);
	Memory_Select_Slave(true);
	HAL_SPI_Transmit(&hspi1, (uint8_t *) &command, 1, 5);
	HAL_SPI_Transmit(&hspi1, (uint8_t *) &data->memory.addr8[2], 1, 5);
	HAL_SPI_Transmit(&hspi1, (uint8_t *) &data->memory.addr8[1], 1, 5);
	HAL_SPI_Transmit(&hspi1, (uint8_t *) &data->memory.addr8[0], 1, 5);
	for (uint8_t i = 0; i < data->size; i++) {
		HAL_SPI_Transmit(&hspi1, &data->data[i], 1, 5);
	}
	Memory_Select_Slave(false);
	Memory_BusyDelay();
	return;
}
