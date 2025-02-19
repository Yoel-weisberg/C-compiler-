import {Peer} from "https://esm.sh/peerjs@1.5.4?bundle-deps"

class Server {
    constructor() {
        this.peer = new Peer();
        this.clients = new Array() // An array of clients
    } 

    getInitialMessage() {
        // return the inital message containg the list of files and the curerent file data
    }

    getCurrentMouseLocation() {
      // return the current mouse location of the user
    }

    monitorChangesOnMonaco() {
        
    }
}