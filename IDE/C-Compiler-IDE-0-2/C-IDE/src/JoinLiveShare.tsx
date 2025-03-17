import { Peer } from "peerjs";
import { useLiveShare } from "./FileContentContext";
type JoinLiveShareProps = {
    peerId: string; // Adjust type based on your actual data type
};

const JoinLiveShare = ({peerId}: JoinLiveShareProps) => {
    var peer = new Peer();
    var conn =  peer.connect(peerId)
    const {fileData, setFileData} = useLiveShare();
    conn.on('open', function(){
        alert("connected");
        conn.on('data', function(data:any){
            setFileData(String(data.data));
        })
    })
};


export default JoinLiveShare;