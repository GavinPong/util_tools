include Makefile.param

TARGET := libutil_tools.so  libutil_tools.a

DIRS_C := 
DIRS_C += $(wildcard ./base64/*.c)
DIRS_C += $(wildcard ./cJSON/*.c)
DIRS_C += $(wildcard ./inirw/*.c)
DIRS_C += $(wildcard ./strfunc/*.c)
DIRS_C += $(wildcard ./list/*.c)

DIRS_CPP := 
DIRS_CPP += $(wildcard ./base64/*.cpp)
DIRS_CPP += $(wildcard ./cJSON/*.cpp)
DIRS_CPP += $(wildcard ./inirw/*.cpp)
DIRS_CPP += $(wildcard ./strfunc/*.cpp)
DIRS_CPP += $(wildcard ./list/*.cpp)

LIBOBJS_C := 
LIBOBJS_C += $(DIRS_C:%.c=%.o)

LIBOBJS_CPP := 
LIBOBJS_CPP += $(DIRS_CPP:%.cpp=%.o)

CFLAGS += 
LDFLAGS += -lpthread

all:
	cd ./base64 && make
	cd ./cJSON && make
	cd ./inirw && make
	cd ./filefunc && make
	cd ./strfunc && make
	cd ./list && make
	make $(TARGET)

libutil_tools.so:
	$(CC)  $(CFLAGS)  $(LIBOBJS_C) $(LIBOBJS_CPP) $(LDFLAGS) -o  $@  -shared -fPIC
	$(STRIP)   $(STRIPFLAGS)  $@

libutil_tools.a:
	$(AR)  rv $@   $(LIBOBJS_C) $(LIBOBJS_CPP)
	$(STRIP)   $(STRIPFLAGS)  $@

install:
	cp  -rf $(TARGET) 


clean:
	cd ./base64 && make clean
	cd ./cJSON && make clean
	cd ./inirw && make clean
	cd ./strfunc && make clean
	cd ./list && make clean
	rm $(TARGET) -rf

distclean:
	cd ./base64 && make distclean
	cd ./cJSON && make distclean
	cd ./inirw && make distclean
	cd ./strfunc && make distclean
	cd ./list && make distclean
	rm $(TARGET) -rf

.PHONY:libutil_tools.so
.PHONY:libutil_tools.a
.PHONY:clean
.PHONY:distclean
.PHONY:install