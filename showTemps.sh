#! /bin/bash

# Quick little script to show CPU and GPU temps on raspberry pi, and format them nicely
# You can also run 'watch ./showTemps.sh' to monitor the temps recurringly

degrees=$'\u00b0'

gpu_temp="$(vcgencmd measure_temp)"
echo GPU Temp: ${gpu_temp:5:4}"${degrees}"C

cpu_temp=$(</sys/class/thermal/thermal_zone0/temp)
cpu_temp=$((cpu_temp/100))
echo CPU Temp: ${cpu_temp:0:2}.${cpu_temp:2:3}"${degrees}"C
