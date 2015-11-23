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


ssh -l "$remoteuser" "$remotecomputer1" "cd $HOME/AOS/Project2;./AOSProject 0" &
ssh -l "$remoteuser" "$remotecomputer2" "cd $HOME/AOS/Project2;./AOSProject 1" &
ssh -l "$remoteuser" "$remotecomputer3" "cd $HOME/AOS/Project2;./AOSProject 2" &
ssh -l "$remoteuser" "$remotecomputer2" "cd $HOME/AOS/Project2;./AOSProject 3" &
ssh -l "$remoteuser" "$remotecomputer4" "cd $HOME/AOS/Project2;./AOSProject 4" &
ssh -l "$remoteuser" "$remotecomputer5" "cd $HOME/AOS/Project2;./AOSProject 5" &
ssh -l "$remoteuser" "$remotecomputer6" "cd $HOME/AOS/Project2;./AOSProject 6" &
ssh -l "$remoteuser" "$remotecomputer7" "cd $HOME/AOS/Project2;./AOSProject 7" &
ssh -l "$remoteuser" "$remotecomputer8" "cd $HOME/AOS/Project2;./AOSProject 8" &
ssh -l "$remoteuser" "$remotecomputer9" "cd $HOME/AOS/Project2;./AOSProject 9" &
