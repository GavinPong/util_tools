include Makefile.param

TARGET := demo_linux

DIRS_C := 
DIRS_C += $(wildcard ./code/base64/*.c)
DIRS_C += $(wildcard ./code/cJSON/*.c)
DIRS_C += $(wildcard ./code/inirw/*.c)
DIRS_C += $(wildcard ./code/strfunc/*.c)
DIRS_C += $(wildcard ./code/list/*.c)
DIRS_C += $(wildcard ./code/error/*.c)
DIRS_C += $(wildcard ./code/cross_platform/*.c)
DIRS_C += $(wildcard ./code/filefunc/*.c)
DIRS_C += $(wildcard ./demo/*.c)

DIRS_CPP := 
DIRS_CPP += $(wildcard ./code/base64/*.cpp)
DIRS_CPP += $(wildcard ./code/cJSON/*.cpp)
DIRS_CPP += $(wildcard ./code/inirw/*.cpp)
DIRS_CPP += $(wildcard ./code/strfunc/*.cpp)
DIRS_CPP += $(wildcard ./code/list/*.cpp)
DIRS_CPP += $(wildcard ./code/error/*.cpp)
DIRS_CPP += $(wildcard ./code/cross_platform/*.cpp)
DIRS_C += $(wildcard ./code/filefunc/*.cpp)
DIRS_CPP += $(wildcard ./demo/*.cpp)

LIBOBJS_C := 
LIBOBJS_C += $(DIRS_C:%.c=%.o)

LIBOBJS_CPP := 
LIBOBJS_CPP += $(DIRS_CPP:%.cpp=%.o)

CFLAGS += 
LDFLAGS += -lpthread -lm

all:
	cd ./code/base64 && make
	cd ./code/cJSON && make
	cd ./code/inirw && make
	cd ./code/filefunc && make
	cd ./code/strfunc && make
	cd ./code/list && make
	cd ./demo && make 
	make $(TARGET)

demo_linux:$(LIBOBJS_C) $(LIBOBJS_CPP)
	echo $(LIBOBJS_C)
	$(CC)  $(CFLAGS)  $(LIBOBJS_C) $(LIBOBJS_CPP) $(LDFLAGS) -o  $@
	$(STRIP)   $(STRIPFLAGS)  $@

install:
	cp  -rf $(TARGET) $(NFS_DIR)


clean:
	cd ./code/base64 && make clean
	cd ./code/cJSON && make clean
	cd ./code/inirw && make clean
	cd ./code/strfunc && make clean
	cd ./code/list && make clean
	cd ./demo && make clean
	cd ./code/filefunc && make clean
	rm $(TARGET) -rf 

distclean:
	cd ./code/base64 && make distclean
	cd ./code/cJSON && make distclean
	cd ./code/inirw && make distclean
	cd ./code/strfunc && make distclean
	cd ./code/list && make distclean
	cd ./demo && make distclean
	cd ./code/filefunc && make distclean
	rm $(TARGET) -rf

.PHONY:libutil_tools.so
.PHONY:libutil_tools.a
.PHONY:clean
.PHONY:distclean
.PHONY:install