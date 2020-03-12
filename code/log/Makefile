include ../Makefile.param

INCS :=	
INCS += -I$(cJSON_SRC_DIR)
INCS += -I$(LIST_SRC_DIR)
INCS += -I$(CROSS_PLATFORM_DIR)
INCS += -I$(ERR_SRC_DIR)

SRCS_C := 
SRCS_C := $(wildcard *.c)

SRCS_CPP := 
SRCS_CPP += $(wildcard *.cpp)

OBJS_C := $(SRCS_C:%.c=%.o)
OBJS_CPP += $(SRCS_CPP:%.cpp=%.o)

all:$(OBJS_C) $(OBJS_CPP)
	

%.o:%.c
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@ 
	
%.o:%.cpp
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

DEF_INCS := $(INCS)
DEF_C = $(SRCS_C:.c=.d)
DEF_CPP = $(SRCS_CPP:.cpp=.d)
$(DEF_C):%.d:%.c
	$(MAKEDEPEND) $(<:.c=.o) $< $(DEF_INCS) > $@
	
$(DEF_CPP):%.d:%.cpp
	$(MAKEDEPEND) $(<:.cpp=.o) $< $(DEF_INCS) > $@
include $(DEF_C) $(DEF_CPP)
depend:
	-rm -rf $(DEF_C)
	-rm -rf $(DEF_CPP)
	$(MAKE) $(DEF_C)
	$(MAKE) $(DEF_CPP)
	
.PHONY:clean
.PHONY:distclean

clean:
	rm *.o -rf

distclean:
	rm *.o *.d -rf
