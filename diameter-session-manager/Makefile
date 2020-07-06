# Define the target executable
TARGET_EXE = diam_sess_mgr

# List of all source files used
DSM_SRC  =
DSM_SRC += dsm_ccdb.c
DSM_SRC += dsm_driver.c
DSM_SRC += dsm_fsm.c
DSM_SRC += dsm_main.c
DSM_SRC += dsm_tdb.c

# Create lists of objects and headers
ALL_DSM_OBJS = $(DSM_SRC:.c=.o) 
ALL_DSM_DEPS = $(DSM_SRC:.c=.d)

# Make standard target and list names
ALL_OBJS = $(addprefix obj/, $(ALL_DSM_OBJS))
ALL_DEPS = $(addprefix obj/, $(ALL_DSM_DEPS))

# Default behaviour: build the target
#
all: $(TARGET_EXE)

# Include the top level make
#
include $(FEP_ROOT)/Rules.mk

# Make the library projects
make_libs:
	@$(MAKE) -s -C $(FEP_ROOT)/diam_lib

# Path to library unit (can override with environment var)
DIAM_LIB_DIR ?= $(FEP_ROOT)/diam_lib

# List the used libraries
LINKLIBS = $(DIAM_LIB_DIR)/lib.a

# Link all the objects to make the target.
$(TARGET_EXE): $(ALL_OBJS) $(ALL_DEPS) $(LINKLIBS) make_libs
	$(LNK) $(LINKFLAGS) -o $@ $(ALL_OBJS) $(LINKLIBS) $(MAPFLAGS)
