#
# useful commands
# 1. Halt target
# monitor halt
#
# 2. Reset and halt the target
# monitor reset 

# Connect
target extended-remote 127.0.0.1:3333

# Set hardware limits
set remote hardware-breakpoint-limit 4
set remote hardware-watchpoint-limit 2

# Im tired of typing 
alias -a mr = monitor reset
alias -a mrh = monitor reset halt
alias -a mh = monitor halt

# Reset on connect
mr

# shortcut to reload file 
define reload
    set confirm off
    d
    file build/remote_pad
    load
    set confirm on
    mr
end

