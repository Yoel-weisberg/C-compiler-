import { Peer, DataConnection } from "peerjs";
import { useLiveShare } from "./FileContentContext";

// Create a function that handles connecting to a peer
// This is NOT a React component, just a regular function
export const connectToPeer = (targetPeerId: string, setFileData: (data: string) => void) => {
  const peer = new Peer();
  
  console.log(`Attempting to connect to peer: ${targetPeerId}`);
  
  const conn = peer.connect(targetPeerId);
  
  conn.on('open', function() {
    console.log('Connection established');
    
    conn.on('data', function(data: any) {
      console.log('Received data:', data.data);
      if (data && data.DATA) {
        setFileData(String(data.da));
      }
    });
  });
  
  conn.on('error', function(err) {
    console.error(`Connection error:`, err);
  });
  
  peer.on('error', function(err) {
    console.error(`Peer error:`, err);
  });
  
  // Return the connection and peer for cleanup purposes
  return { conn, peer };
};