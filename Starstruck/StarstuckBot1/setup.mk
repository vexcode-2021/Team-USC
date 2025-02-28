# uncomment these if running from default project location
# Path to ChibiOS
#CHIBIOS    = c:/Projects/convex/ChibiOS_2.6.2
# Path to convex
#CONVEX     = c:/Projects/convex/cortex

# uncomment to use the optional code like the smart motor library
CONVEX_OPT  = yes

# User C code files
VEXUSERSRC = vexuser.c \
             vexdefault.c \
             ../Common/easing.c \
             ../Common/takebackhalf.c \
             ../Common/common.c

# Uncomment and add/modify user include files
#VEXUSERINC = myfile.h
