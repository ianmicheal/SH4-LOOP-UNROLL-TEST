
# Ian micheal expanded loop unrolling test based on pcercuei/sh4_gcc_test_unroll.c
#17/06/25 20:05
OBJS = looptest.o

SCRAMBLED = 1st_read.bin

KOS_LOCAL_CFLAGS = -I$(KOS_BASE)/addons/zlib

all: rm looptest.cdi

include $(KOS_BASE)/Makefile.rules

clean:
	-rm -f $(SCRAMBLED)
	-rm -f looptest.bin
	-rm -f looptest.cdi
	-rm -f looptest.iso
	-rm -f looptest.elf $(OBJS)
	-rm -f romdisk_boot.*

rm:
	-rm -f $(SCRAMBLED)
	-rm -f looptest.bin
	-rm -f looptest.cdi
	-rm -f looptest.iso
	-rm -f looptest.elf
	-rm -f romdisk_boot.*

looptest.cdi: $(SCRAMBLED)
	sh-elf-objcopy -R .stack -O binary looptest.elf looptest.bin
	$(KOS_BASE)/utils/scramble/scramble looptest.bin 1st_read.bin
	mkisofs -G $(KOS_BASE)/../IP.BIN -C 0,11702 -J -l -r -o looptest.iso .
	cdi4dc looptest.iso looptest.cdi

$(SCRAMBLED): looptest.elf
	sh-elf-objcopy -R .stack -O binary looptest.elf looptest.bin
	$(KOS_BASE)/utils/scramble/scramble looptest.bin $(SCRAMBLED)

looptest.elf: $(OBJS) romdisk_boot.o 
	$(KOS_CC) $(KOS_CFLAGS) $(KOS_LDFLAGS) -o $@ $(KOS_START) $^ -ltremor  -lpng -lz -lm $(KOS_LIBS)

romdisk_boot.img:
	$(KOS_GENROMFS) -f $@ -d romdisk_boot -v

romdisk_boot.o: romdisk_boot.img
	$(KOS_BASE)/utils/bin2o/bin2o $< romdisk_boot $@
