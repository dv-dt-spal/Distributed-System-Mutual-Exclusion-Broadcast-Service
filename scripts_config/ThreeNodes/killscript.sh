remoteuser=sxp142730
remotecomputer1=dc01.utdallas.edu
remotecomputer2=dc02.utdallas.edu
remotecomputer3=dc03.utdallas.edu


ssh -l "$remoteuser" "$remotecomputer1" "killall -u $remoteuser" &
ssh -l "$remoteuser" "$remotecomputer2" "killall -u $remoteuser" &
ssh -l "$remoteuser" "$remotecomputer3" "killall -u $remoteuser" &