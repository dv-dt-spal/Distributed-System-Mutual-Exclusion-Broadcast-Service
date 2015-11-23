remoteuser=sxp142730
remotecomputer1=dc01.utdallas.edu
remotecomputer2=dc02.utdallas.edu
remotecomputer3=dc03.utdallas.edu



ssh -l "$remoteuser" "$remotecomputer3" "cd $HOME/AOS/Project2;./AOSProject 2" &
ssh -l "$remoteuser" "$remotecomputer2" "cd $HOME/AOS/Project2;./AOSProject 1" &
ssh -l "$remoteuser" "$remotecomputer1" "cd $HOME/AOS/Project2;./AOSProject 0" &
