# **********************************************
# Application Makefile Config
# **********************************************
TARGET := client

LOCAL_INCS := . ./include	\
				../../include
LOCAL_SRCS := .

CC ?= gcc

LOCAL_CFLAGS :=
LOCAL_LDFLAGS := -lpthread  -lm -L./

# **********************************************
# Common CFLAGS & LDFLAGS
# **********************************************
LOCAL_CFLAGS += -Wno-multichar -fno-exceptions -ffunction-sections -fdata-sections -funwind-tables \
			-fno-short-enums -fno-builtin-sin -fno-strict-volatile-bitfields -fmessage-length=0 \
			-fgcse-after-reload -frerun-cse-after-loop -frename-registers -fomit-frame-pointer \
			-fno-strict-aliasing -flto -m32
LOCAL_LDFLAGS += -Wl,--gc-sections -Wl,--no-undefined

# **********************************************
# Common Makefile Rules (C Source Files)
# **********************************************
CFLAGS += $(LOCAL_CFLAGS) -Wall $(foreach dir, $(LOCAL_INCS), -I$(dir))
LDFLAGS += $(LOCAL_LDFLAGS)

ifeq ($(suffix $(TARGET)),.so)
	CFLAGS += -fPIC
	LDFLAGS += -shared
endif

ifneq ($(wildcard config.h),)
	CFLAGS += -include config.h
endif

SRCS  := $(foreach file, $(foreach dir, $(LOCAL_SRCS), $(wildcard $(dir)/*.S)), $(subst ./, , $(file))) \
                   $(foreach file, $(foreach dir, $(LOCAL_SRCS), $(wildcard $(dir)/*.c)), $(subst ./, , $(file))) \
                   $(foreach file, $(foreach dir, $(LOCAL_SRCS), $(wildcard $(dir)/*.cpp)), $(subst ./, , $(file)))

OBJS := $(patsubst %.S,%.o, $(patsubst %.c,%.o, $(patsubst %.cpp, %.o, $(SRCS))))

.PHONY : all clean

all : $(TARGET)

$(TARGET) : $(OBJS)
ifeq ($(suffix $(TARGET)),.a)
	$(AR) curv $@ $(OBJS)
	$(RANLIB) $(TARGET)
else
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)
endif

clean :
	rm -rf $(OBJS)
	rm -rf $(TARGET)

