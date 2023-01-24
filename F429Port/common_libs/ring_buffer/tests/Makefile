TARGET = test
BUILD_DIR = build

CPP_SOURCES = \
main.c \
../ring_buffer.c

OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(CPP_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(CPP_SOURCES)))

all: $(TARGET)

$(BUILD_DIR)/%.o : %.c | $(BUILD_DIR)
	gcc -c $< -o $@

$(TARGET) : $(OBJECTS) | $(BUILD_DIR)
	gcc -o $(BUILD_DIR)/$(TARGET) $(BUILD_DIR)/*.o
	@echo BUILD Done. Jolly good!

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

clean:
	rm -fR $(BUILD_DIR)
