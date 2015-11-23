remoteuser=sxp142730
remotecomputer1=dc17.utdallas.edu
remotecomputer2=dc18.utdallas.edu
remotecomputer3=dc19.utdallas.edu
remotecomputer4=dc20.utdallas.edu
remotecomputer5=dc21.utdallas.edu



ssh -l "$remoteuser" "$remotecomputer5" "cd $HOME/AOS/Project2;./AOSProject 4" &
ssh -l "$remoteuser" "$remotecomputer4" "cd $HOME/AOS/Project2;./AOSProject 3" &
ssh -l "$remoteuser" "$remotecomputer3" "cd $HOME/AOS/Project2;./AOSProject 2" &
ssh -l "$remoteuser" "$remotecomputer2" "cd $HOME/AOS/Project2;./AOSProject 1" &
ssh -l "$remoteuser" "$remotecomputer1" "cd $HOME/AOS/Project2;./AOSProject 0" &
