TARGET ?= server.out client.out
SRC_DIRS ?= ./src

SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
OBJS := $(addsuffix .o,$(basename $(SRCS)))
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP

OFLAGS := -pthread -lpthread -Wall

OTHERLIBS := -lrt

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(OFLAGS) $(OBJS) -o $@ $(LOADLIBES) $(LDLIBS) $(OTHERLIBS)

.PHONY: clean
clean:
	$(RM) $(TARGET) $(OBJS) $(DEPS)

-include $(DEPS)
