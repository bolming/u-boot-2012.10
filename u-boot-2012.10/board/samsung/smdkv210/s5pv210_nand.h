#ifndef	S5PV210_NAND_H
#define	S5PV210_NAND_H

#define	S5PV210_NAND_BASE	0xB0E00000 

#define	CONFIG_S5PV210_TACLS	1
#define	CONFIG_S5PV210_TWRPH0	4
#define	CONFIG_S5PV210_TWRPH1	1

/* NAND FLASH Configuration */
struct s5pv210_nand {
        u32     nfconf;
	u32	nfcont;
        u32     nfcmd;
        u32     nfaddr;
        u32     nfdata;
	/* ecc */
	u32	nfmeccd[2];
	u32	nfseccd;
	/* lock */
	u32	nfsblk;
	u32	nfeblk;

        u32     nfstat;

	u32	nfeccerr[2];
        u32     nfmecc[2];
        u32     nfsecc;
	
	u32	nfmlcbitpt;
};

static inline struct s5pv210_nand *s5pv210_get_base_nand(void)
{
       return (struct s5pv210_nand *)S5PV210_NAND_BASE;
}

#endif /* S5PV210_NAND_H */
