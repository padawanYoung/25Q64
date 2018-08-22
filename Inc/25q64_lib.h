#include "main.h"
#include "stdlib.h"
#include "stdbool.h"

typedef enum Memory_SPI_Instructions {
	WriteEnable = 0x06,
	VolatileSRWriteEnable = 0x50,
	WriteDisable = 0x04,
	ReadStatusRegister1 = 0x05,
	ReadStatusRegister2 = 0x35,
	ReadStatusRegister3 = 0x15,
	WriteStatusRegister1 = 0x01,
	WriteStatusRegister2 = 0x31,
	WriteStatusRegister3 = 0x11,
	PageProgram = 0x02,
	FastPageProgram = 0xf2,
	SectorErase_4kb = 0x20,
	SectorErase_32kb = 0x52,
	SectorErase_64kb = 0xd8,
	ChipErase = 0xc7,
	ProgramSuspend = 0x75,
	ProgramResume = 0x74,
	PowerDown = 0xb9,
	ReleasePowerDown = 0xAB,
	ReadData = 0x03,
	FastRead = 0x0b,
	EnableReset = 0x66,
	Reset = 0x99
} SPI_FlashData_Instructions;

typedef union {
	uint32_t addr32;
	uint8_t addr8[4];
} Memory;

typedef struct {
	uint8_t *data;
	size_t size;
	Memory memory;
} DataStructure;

void Memory_Select_Slave(bool action);

void Memory_Write_Permission(bool instruction);

uint8_t Memory_ReadStatusRegister(uint8_t number);

void Memory_WriteStatusRegister(uint8_t number, uint8_t value);

void Memory_BusyDelay();

void Memory_ChipErase();

void Memory_BlockErase_4kb(uint32_t address);

uint8_t Memory_ReadDataBytes(uint32_t addres);

void Memory_PowerDown(bool state);

void Memory_Reset();

void Memory_WriteData(DataStructure * data);

void Memory_ReadManufacture();