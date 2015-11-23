remoteuser=sxp142730
remotecomputer1=dc01.utdallas.edu
remotecomputer2=dc33.utdallas.edu
remotecomputer3=dc21.utdallas.edu
remotecomputer4=dc22.utdallas.edu
remotecomputer5=dc45.utdallas.edu
remotecomputer6=dc35.utdallas.edu
remotecomputer7=dc27.utdallas.edu
remotecomputer8=dc17.utdallas.edu
remotecomputer9=dc03.utdallas.edu


ssh -l "$remoteuser" "$remotecomputer1" "killall -u $remoteuser" &
ssh -l "$remoteuser" "$remotecomputer2" "killall -u $remoteuser" &
ssh -l "$remoteuser" "$remotecomputer3" "killall -u $remoteuser" &
ssh -l "$remoteuser" "$remotecomputer2" "killall -u $remoteuser" &
ssh -l "$remoteuser" "$remotecomputer4" "killall -u $remoteuser" &
ssh -l "$remoteuser" "$remotecomputer5" "killall -u $remoteuser" &
ssh -l "$remoteuser" "$remotecomputer6" "killall -u $remoteuser" &
ssh -l "$remoteuser" "$remotecomputer7" "killall -u $remoteuser" &
ssh -l "$remoteuser" "$remotecomputer8" "killall -u $remoteuser" &
ssh -l "$remoteuser" "$remotecomputer9" "killall -u $remoteuser" &