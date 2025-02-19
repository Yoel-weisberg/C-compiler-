import {Peer} from "https://esm.sh/peerjs@1.5.4?bundle-deps"
import ClientCommunicater from "./ClientCommunicater";
import { RequestCodes, ResponseCodes } from "../PacketCodes";
import Change from "./Chnages";
export default class Server {
    constructor() {
        this.peer = new Peer();
        this.peerId = null;
        this.areThereChnages = false;
        const currentChange = null;
        const connections = new Set();
        this.peer.on('open', (id) => {
            this.peerId = id;
            this.mainLoop();
        })
    } 

    mainLoop() {
        this.peer.on('connection', (conn) =>{
            //firstly we are going to send the user the basic information and then add him 
            // to the broadcost list
            conn.send(this.makeResponse(ResponseCodes.INITIAL_FETCH, this.getInitialMessage()));
            conn.send(this.makeResponse(ResponseCodes.INITIAL_FETCH, this.getFileContent()))
            // finally we are adding the connection to the broadcost list
            this.connections.add(conn);
            conn.on('data', (data) => {
                // handles a case when the client sends us data - which shoudl never happen
            })
        })
    }

    broadcost(change) {
        for (const conn of this.connections) {
            conn.send(this.makeResponse(ResponseCodes.CHANGE_DETECTED, change.toJSON()));
        }
    }
    getInitialMessage() {
        // return the inital message containg the list of files and the curerent file data
    }

    // this function should use the IModel.getValue() function
    getFileContent() {
        return "some file content";
    }
    makeResponse(ResponseCode, ResponseJSON) {
        return {PacketCode: ResponseCode, data: ResponseJSON}
    }
    getCurrentMouseLocation() {
      // return the current mouse location of the user
      // reurns the mouse location in a string format  using the 
      // toJson function on the mouse location
    }

    monitorChangesOnMonaco() {
        // this function would get the return the changes dont to the open file 
        // or if the window had changed
        // if so it should call the broadcost function with the change being the change done
    }
}