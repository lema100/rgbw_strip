#include "update.h"
#include "main.h"

int main(void)
{
	update_ctx_t s_up;
	update_state_t res = update_check(&s_up);
	if (res == UPDATE_CRC_MISMATCH)
	{
		FLASH_EraseInitTypeDef pEraseInit;
		uint32_t len = s_up.end_add - s_up.start_add;
		uint32_t PageError;

		HAL_FLASH_Unlock();
		for (uint32_t i = UPDATE_APP_START; i < (UPDATE_APP_START + len); i += FLASH_PAGE_SIZE)
		{
			pEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
			pEraseInit.Banks = FLASH_BANK_1;
			pEraseInit.NbPages = 1;
			pEraseInit.PageAddress = i;

			HAL_FLASHEx_Erase(&pEraseInit, &PageError);
		}
		for (uint32_t i = 0; i < len; i += 4)
		{
				HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, UPDATE_APP_START + i, *(uint32_t *)(((uint8_t *)s_up.start_add + sizeof(update_ctx_t)) + i));
		}
		HAL_FLASH_Lock();
		NVIC_SystemReset();
	}
	BOOT_boot(UPDATE_APP_START);
}

