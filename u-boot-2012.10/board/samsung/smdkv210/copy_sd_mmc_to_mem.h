#ifndef COPY_SD_MMC_TO_MEM_H
#define COPY_SD_MMC_TO_MEM_H

#define	V210_SDMMC_BASE	( *((volatile unsigned long *) 0xD0037488) )
#define SDMASYSAD0	0xEB000000
#define SDMASYSAD2	0xEB200000

#define _copy_sd_mmc_to_mem(z,a,b,c,e) \
	(((unsigned int (*)(int, unsigned int, unsigned short, unsigned int*, unsigned int))(*((unsigned int *)0xD0037F98)))(z,a,b,c,e))

unsigned int copy_sd_mmc_to_mem(unsigned int start, unsigned short size, unsigned int *dest)
{
	unsigned long ch = V210_SDMMC_BASE;

	int ret = 0;
	if(SDMASYSAD0 == ch){
		ret = _copy_sd_mmc_to_mem(0, start, size, dest, 0);
	}else if(SDMASYSAD2 == ch){
		ret = _copy_sd_mmc_to_mem(2, start, size, dest, 0);
	}else{
		return 0;
	}
	
	return ret;
}

#endif /* COPY_SD_MMC_TO_MEM_H */
