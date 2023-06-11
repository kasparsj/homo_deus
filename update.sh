host="hd@homodeus.local"
path="/home/hd/homodeus"
#host="kaspars.cc"
#path="/home/root/homodeus"
ssh -t $host "rm -rf $path"
scp -r SuperCollider $host:$path
#rsync -r SuperCollider $host:$path
ssh -t $host 'sudo reboot'
