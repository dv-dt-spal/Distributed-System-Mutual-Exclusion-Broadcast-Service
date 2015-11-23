remoteuser=sxp142730
remotecomputer1=dc17.utdallas.edu
remotecomputer2=dc18.utdallas.edu
remotecomputer3=dc19.utdallas.edu
remotecomputer4=dc20.utdallas.edu
remotecomputer5=dc21.utdallas.edu


ssh -l "$remoteuser" "$remotecomputer1" "killall -u $remoteuser" &
ssh -l "$remoteuser" "$remotecomputer2" "killall -u $remoteuser" &
ssh -l "$remoteuser" "$remotecomputer3" "killall -u $remoteuser" &
ssh -l "$remoteuser" "$remotecomputer4" "killall -u $remoteuser" &
ssh -l "$remoteuser" "$remotecomputer5" "killall -u $remoteuser" &
