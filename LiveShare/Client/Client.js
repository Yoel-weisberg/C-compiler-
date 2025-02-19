import {Peer} from "https://esm.sh/peerjs@1.5.4?bundle-deps"
import { ResponseCodes } from "../PacketCodes";

export default class Client {
    constructor (peerId){
        this.peerId = peerId;
        this.peer = new Peer()
        this.conn = this.peer.connect(this.peerId);

        conn.on('open', function() {
            // Receive messages
            conn.on('data', function(data) {
              const packetCode = data.PacketCode;
              switch(packetCode) {
                case ResponseCodes.INITIAL_FETCH:
                    console.log('Received INITIAL_FETCH');
                    break;
                case ResponseCodes.FETCH_FILE:
                    console.log('Received FETCH_FILE');
                    break;
                case ResponseCodes.CHANGE_DETECTED:
                    console.log('Received CHANGE_DETECTED, change: ', data.data);
                    break;
                case ResponseCodes.FILE_UPDATED:
                default:
              }
            });
        
          });
    }

}