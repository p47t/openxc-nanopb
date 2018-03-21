ccflags-y := -std=gnu99 -Wno-declaration-after-statement -I$(src) -DPB_SYSTEM_HEADER=\"pb_system.h\"
EXTRA_CFLAGS := -I$(src)/nanopb

PBSRCS := $(wildcard *.proto)
PBOBJS := $(PBSRCS:.proto=.pb.o)

obj-m += openxc.o
openxc-objs := main.o nanopb/pb_encode.o nanopb/pb_decode.o nanopb/pb_common.o $(PBOBJS)

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

watch:
	@while true; do \
	    echo \\n━━━━━━━━; \
	    make; \
	    inotifywait -qre close_write .; \
	done
