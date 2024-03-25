# 
# Usage: To re-create this platform project launch xsct with below options.
# xsct D:\Vitis\embedded-mo8\mo8-zender-platform\platform.tcl
# 
# OR launch xsct and run below command.
# source D:\Vitis\embedded-mo8\mo8-zender-platform\platform.tcl
# 
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

platform create -name {mo8-zender-platform}\
-hw {C:\Users\Jochem\GitHub\Fontys\mo8-digitaal-zender\mo8-digitaal-zender-v0.2.xsa}\
-proc {ps7_cortexa9_0} -os {standalone} -out {D:/Vitis/embedded-mo8}

platform write
platform generate -domains 
platform active {mo8-zender-platform}
platform clean
platform generate
