
#ifndef __ASM_ARCH_MEMCONFIG_H_
#define __ASM_ARCH_MEMCONFIG_H_
#include "sc_config.h"


#ifndef  _NAND_FLASH_
#define BSPCONF_SDRAM_BASE 0x00000000 /* ?? */

#define BSPCONF_BLOCK_SIZE			(64*1024)

//FLASH
#define BSPCONF_FLASH_BASE 0x02000000
#define FLASH_BASE BSPCONF_FLASH_BASE

#define BSPCONF_FLASH_SIZE 0x01000000
#define FLASH_SIZE BSPCONF_FLASH_SIZE

/* FLASH Memory Map */
#ifdef _DEF_CONF_FILE_IN_FLASH_
#define BSPCONF_FLASH_PARTITIONS        8
#else
#define BSPCONF_FLASH_PARTITIONS        7
#endif
typedef enum
{
	PART_IDX_ALL = 0,
	PART_IDX_MAC,
	PART_IDX_CONF,
	PART_IDX_RESERV,
	PART_IDX_NORMAL,	
	PART_IDX_FS,
	PART_IDX_DEBUG,
#ifdef _DEF_CONF_FILE_IN_FLASH_
	PART_IDX_DEF_CONF,
#endif
#ifdef _SC_SUPPORT_DUAL_IMAGE_
	PART_IDX_RESERV_PART,
	PART_IDX_UPG,
#endif
	MAX_PART_IDX
}PartIdx;

#define BSPCONF_FW_FLASH_OFFSET		0x00000000
#ifdef _SPI_NOR_FLASH_
#define BSPCONF_UBL_FLASH_OFFSET		BSPCONF_FW_FLASH_OFFSET
#define BSPCONF_UBL_FLASH_SIZE		(32*1024)
#define BSPCONF_BTLDR_FLASH_OFFSET     (BSPCONF_UBL_FLASH_OFFSET+BSPCONF_UBL_FLASH_SIZE)
#else
#define BSPCONF_BTLDR_FLASH_OFFSET      BSPCONF_FW_FLASH_OFFSET
#endif
#define BSPCONF_BTLDR_FLASH_SIZE        	(256*1024)

#ifdef _SC_SUPPORT_DUAL_IMAGE_
#define BSPCONF_DNDATER_FLASH_OFFSET      (BSPCONF_BTLDR_FLASH_OFFSET+BSPCONF_BTLDR_FLASH_SIZE)
#define BSPCONF_DNDATER_FLASH_SIZE          (124*1024)

#define BSPCONF_PARTTAB_FLASH_OFFSET      (BSPCONF_DNDATER_FLASH_OFFSET+BSPCONF_DNDATER_FLASH_SIZE)
#define BSPCONF_PARTTAB_FLASH_SIZE          (4*1024)

#define BSPCONF_MAC_FLASH_OFFSET		(BSPCONF_PARTTAB_FLASH_OFFSET+BSPCONF_PARTTAB_FLASH_SIZE)
#define BSPCONF_MAC_FLASH_SIZE          	(32*1024) 
#else
#define BSPCONF_DNDATER_FLASH_OFFSET      (BSPCONF_BTLDR_FLASH_OFFSET+BSPCONF_BTLDR_FLASH_SIZE)
#define BSPCONF_DNDATER_FLASH_SIZE          (128*1024)

#define BSPCONF_MAC_FLASH_OFFSET		(BSPCONF_DNDATER_FLASH_OFFSET+BSPCONF_DNDATER_FLASH_SIZE)
#define BSPCONF_MAC_FLASH_SIZE          	(32*1024) 

#endif
#define BSPCONF_MAC6BYTE_OFFSET        	(BSPCONF_MAC_FLASH_OFFSET+BSPCONF_MAC_FLASH_SIZE-6)

#define BSPCONF_WIRELESS_CODE_OFFSET 		(BSPCONF_MAC_FLASH_OFFSET+ 4*1024)
#define BSPCONF_WIRELESS_CODE_SIZE		1024

#define BSPCONF_CONF_FLASH_OFFSET       (BSPCONF_MAC_FLASH_OFFSET+BSPCONF_MAC_FLASH_SIZE)
#define BSPCONF_CONF_FLASH_SIZE      		(128*1024)

#define BSPCONF_DEBUG_FLASH_OFFSET	(BSPCONF_CONF_FLASH_OFFSET+BSPCONF_CONF_FLASH_SIZE)
#define BSPCONF_DEBUG_FLASH_SIZE		(320*1024)

#ifdef _DEF_CONF_FILE_IN_FLASH_
#define BSPCONF_DEF_CONF_FLASH_OFFSET	(BSPCONF_DEBUG_FLASH_OFFSET+BSPCONF_DEBUG_FLASH_SIZE)
#define BSPCONF_DEF_CONF_FLASH_SIZE		(128*1024)

#define BSPCONF_RESERVED_FLASH_OFFSET	(BSPCONF_DEF_CONF_FLASH_OFFSET+BSPCONF_DEF_CONF_FLASH_SIZE)
#define BSPCONF_RESERVED_FLASH_SIZE		(160*1024)
#else
#define BSPCONF_RESERVED_FLASH_OFFSET	(BSPCONF_DEBUG_FLASH_OFFSET+BSPCONF_DEBUG_FLASH_SIZE)
#define BSPCONF_RESERVED_FLASH_SIZE		(288*1024)
#endif

#define BSPCONF_KERNEL_FLASH_OFFSET     (BSPCONF_RESERVED_FLASH_OFFSET+BSPCONF_RESERVED_FLASH_SIZE)
#define BSPCONF_KERNEL_FLASH_SIZE       		(2*1024*1024)

#define BSPCONF_FS_FLASH_OFFSET         (BSPCONF_KERNEL_FLASH_OFFSET+BSPCONF_KERNEL_FLASH_SIZE)
#define BSPCONF_FS_FLASH_SIZE           		(FLASH_SIZE-BSPCONF_FS_FLASH_OFFSET)

#define BSPCONF_CHKSUM_1_FLASH_SIZE            4
#define BSPCONF_CHKSUM_1_FLASH_OFFSET       (FLASH_SIZE-BSPCONF_CHKSUM_1_FLASH_SIZE)

#define BSPCONF_CHKSUM_2_FLASH_SIZE            4
#define BSPCONF_CHKSUM_2_FLASH_OFFSET       (BSPCONF_CHKSUM_1_FLASH_OFFSET-BSPCONF_CHKSUM_2_FLASH_SIZE)

#define BSPCONF_FW_RELESE_SIZE		11
#define BSPCONF_FW_RELESE_OFFSET		(BSPCONF_CHKSUM_2_FLASH_OFFSET-BSPCONF_FW_RELESE_SIZE)

#define BSPCONF_PID_FLASH_SIZE            70
#define BSPCONF_PID_FLASH_OFFSET       	(BSPCONF_FW_RELESE_OFFSET-BSPCONF_PID_FLASH_SIZE)

#define BSPCONF_FW_TIME_FLASH_SIZE            28
#define BSPCONF_FW_TIME_FLASH_OFFSET       (BSPCONF_PID_FLASH_OFFSET-BSPCONF_FW_TIME_FLASH_SIZE)

#define BSPCONF_ENG_VER_FLASH_SIZE            2
#define BSPCONF_ENG_VER_FLASH_OFFSET       (BSPCONF_FW_TIME_FLASH_OFFSET-BSPCONF_ENG_VER_FLASH_SIZE)

#define BSPCONF_KEN_OFS_FLASH_SIZE            4
#define BSPCONF_KEN_OFS_FLASH_OFFSET       (BSPCONF_ENG_VER_FLASH_OFFSET-BSPCONF_KEN_OFS_FLASH_SIZE)

#define BSPCONF_KEN_SIZE_FLASH_SIZE            4
#define BSPCONF_KEN_SIZE_FLASH_OFFSET       (BSPCONF_KEN_OFS_FLASH_OFFSET-BSPCONF_KEN_SIZE_FLASH_SIZE)

// fs real size
#define BSPCONF_FSSIZE_FLASH_SIZE            	4
#define BSPCONF_FSSIZE_FLASH_OFFSET      	(BSPCONF_KEN_SIZE_FLASH_OFFSET-BSPCONF_FSSIZE_FLASH_SIZE)

#define BSPCONF_EXTRAINFO_FLASH_SIZE           EXTRA_INFO_SIZE_IN_FLASH
#define BSPCONF_EXTRAINFO_FLASH_OFFSET      (BSPCONF_FSSIZE_FLASH_OFFSET-BSPCONF_EXTRAINFO_FLASH_SIZE)

#ifdef _SC_FLASHMAP_CARE_REAL_KERNEL_FS_SIZE_
// product info offset
#define BSPCONF_PRODUCTINFO_FLASH_OFFSET	(FLASH_SIZE - BSPCONF_BLOCK_SIZE)

typedef struct Upgsize_INFO
{
	int fs_real_size;	// real filesystem size
	int kernel_real_size;	// kernel real size
	int kernel_offset;		// kernel offset
} __attribute__ ((packed))  Upgsize_INFO;
#endif

#else

//#define MAX_NFLASH_PARTITION 32

typedef enum
{
	PART_IDX_FW = 0,
	PART_IDX_CONF0,
	PART_IDX_CONF1,
	PART_IDX_MAC,	
	PART_IDX_NFW,	
	PART_IDX_FS,
	PART_IDX_PINFO,
	PART_IDX_UPGRADE,
	PART_IDX_DEBUG,
	PART_IDX_UPGLOG,
	PART_IDX_RESERVED,
	PART_IDX_CALIBRATION,
	PART_IDX_DUMP,
	PART_IDX_BLD,
	PART_IDX_BK1,
	MAX_PART_IDX
}PartIdx;

typedef enum
{
	BOOTSTRAP_AREA_ID = 0x0001,	//0x0001
	BOOTLOADER_AREA_ID,	//0x0002
	DOWNLOADER_AREA_ID,	//0x0003
	KERNEL_AREA_ID,	//0x0004
	FS_AREA_ID,	//0x05
	PRODUCTINFO_AREA_ID, //0x06
	PT_AREA_ID,	//0x07
	// Special ID
	FIRMWARE_ID = 0xFF01,	// 0xff01
	AT2_KERNEL_FS_ID,	// 0xff02
}AreaID;
typedef enum
{
        IMAGE1_ID = 0x0001,     //0x0001
        IMAGE2_ID,      //0x0002
        RESERVED_ID,    //0x0003
        CONFIGURATION_ID,       //0x0004
        FACTORY_ID,     //0x05
	CALIBRATION_ID,//0x06
	PARTITION_MAX_ID//0x07
}PartID;


#define BLOCK_SIZE 				(128*1024)
#define NFLASH_FW_OFS			0x0
#define NFLASH_FW_SIZE			0x1400000

#define BSD_SIZE					(4*BLOCK_SIZE)
#define CIS_SIZE					(BLOCK_SIZE)
#define NFLASH_AT1_OFS			(NFLASH_FW_OFS+5*BSD_SIZE+CIS_SIZE)

#define NFLASH_DEBUG_OFS		(NFLASH_FW_OFS+NFLASH_FW_SIZE)
#define NFLASH_DEBUG_SIZE		0x200000

#define NFLASH_CONF_OFS			(NFLASH_DEBUG_OFS+NFLASH_DEBUG_SIZE)
#define NFLASH_CONF_SIZE		0x900000

#define NFLASH_MAC_OFS			(NFLASH_CONF_OFS+NFLASH_CONF_SIZE)
#define NFLASH_MAC_SIZE			0x100000

#define NFLASH_FS_OFS
#define NFLASH_FS_SIZE

#define NFLASH_WIRELESS_OFS		(NFLASH_FS_OFS+1024)
#define NFLASH_WIRELESS_SIZE 1024

#define BSPCONF_FLASH_SIZE 		NFLASH_FW_SIZE

#define MAX_AREA_INTAB_NUM			20
#define MAGIC_NUMBER_VALID 			(0xA1ACED00)
#define UBL_RESERVED_BLOCK		21
#define MAGIC_SEARCH_BLOCK		4
#define MD5_RESULT_LEN          16
#define MAX_PARTITION_NUM	20

typedef struct
{
	unsigned char md5[MD5_RESULT_LEN];
	unsigned short version;
	unsigned short num;
	unsigned short seq_no;
}__attribute__ ((packed)) AreaTabHeader;
typedef struct
{
	unsigned short id;
	unsigned short version;
	unsigned int length;
}__attribute__ ((packed)) AreaTabEntry;
typedef struct
{
	AreaTabHeader header;
	AreaTabEntry entrys[MAX_AREA_INTAB_NUM];
}__attribute__ ((packed)) AreaTabConf;
typedef struct
{
	unsigned char md5[MD5_RESULT_LEN];					//The MD5 value for all the new AT content excluding it self.
	unsigned short version;						
	unsigned short num;
	unsigned short seqno;
}__attribute__ ((packed)) NewAreaTabHeader;

typedef struct
{
	NewAreaTabHeader header;
	AreaTabEntry entrys[MAX_AREA_INTAB_NUM];
}__attribute__ ((packed)) NewAreaTabConf;
typedef struct PartTabHeader_s
{
        unsigned short version;
        unsigned char entry_num;
} __attribute__((packed))  PartTabHeader;

typedef struct PartTabEntry_s
{
        unsigned short id;
        unsigned short version;
        unsigned int offset;
        unsigned int length;
} __attribute__((packed))  PartTabEntry;

typedef struct PartTabConf_s
{
        PartTabHeader header;
        PartTabEntry entrys[MAX_PARTITION_NUM];
} __attribute__((packed))  PartTabConf;

#endif

#ifdef _SC_SUPPORT_DUAL_IMAGE_
#define FLASH_FW_OFFSET         0x0

#ifdef _NAND_FLASH_
#define FLASH_BLOCK_SIZE        131072
#define FLASH_PAGE_SIZE         2048
#define FLASH_OOB_SIZE          64
#define FLASH_BLOCK_OOB_SIZE    ((FLASH_BLOCK_SIZE) + ((FLASH_BLOCK_SIZE)/(FLASH_PAGE_SIZE) * (FLASH_OOB_SIZE)))
#define FLASH_MAX_BLOCK_NUM 	1024
#define FLASH_FW_SIZE			0x10000000
#define NFLASH_DUMP_SIZE		0x10000000

#define FLASH_BST_OFFSET        FLASH_FW_OFFSET
#define FLASH_BST_SIZE          FLASH_BLOCK_SIZE
#define FLASH_PTB_OFFSET        (FLASH_BST_OFFSET+FLASH_BST_SIZE)
#define FLASH_PTB_SIZE          FLASH_BLOCK_SIZE
#define FLASH_SC_PTB_SIZE      	FLASH_BLOCK_SIZE
#define FLASH_AT_SIZE           FLASH_BLOCK_SIZE
#define FLASH_DEBUG_SIZE 0x200000
#define FLASH_UPGLOG_SIZE 0x140000
#define FLASH_CALIBRATION_SIZE 0x400000

#else
#define FLASH_FW_SIZE           0x2000000
#define NFLASH_DUMP_SIZE		0x2000000
#endif

#endif

#endif /* __ASM_ARCH_MEMCONFIG_H_ */