ipcs -sa | awk '/^s / { if ($9 == 0) { print $2 }}' | xargs -n 1 ipcrm -s
ipcs -ma | awk '/^m / { if ($9 == 0) { print $2 }}' | xargs -n 1 ipcrm -m
ipcs -qa | awk '/^q / { if ($9 == 0) { print $2 }}' | xargs -n 1 ipcrm -q